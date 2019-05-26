#include <wk/task.h>
#include <wk/cpu.h>
#include <wk/delay.h>
#include <wk/err.h>
#include <wk/sem.h>
#include <wk/mm.h>
#include <wk/clk.h>
#include <wk/device.h>
#include <init/init.h>

#include "com.h"

#define COM_TASK_PRIO       22
#define COM_TASK_STACK_SIZE 512
#define COM_TASK_TICK       10

static struct task_struct_t *com_task;
struct device *flash_dev = NULL;

struct com_data {
    struct device *dev;
    sem_t *read_sem;
    sem_t *write_sem;
};

struct com_data *com_data;

void com_read_complete(__maybe_unused struct device *dev, __maybe_unused size_t size)
{
    //pr_info("%s entry\r\n", __func__);
    sem_send(com_data->read_sem, 1);
}
void com_write_complete(__maybe_unused struct device *dev, __maybe_unused const void *buffer)
{
    //pr_info("%s entry\r\n", __func__);
    sem_send(com_data->write_sem, 1);
}

void com_read_data_all_wait(uint8_t *buf, size_t size)
{
    com_data->dev->ops.read(com_data->dev, 0, buf, size);
    sem_get(com_data->read_sem);
}

int com_read_data(uint8_t *buf, size_t size)
{
    int ret;

    com_data->dev->ops.read(com_data->dev, 0, buf, size);
    ret = sem_get_timeout(com_data->read_sem, sec_to_tick(2));
    if (ret != 0) {
        //pr_err("winusb read data timedout\r\n");
        return -ETIMEDOUT;
    }

    return 0;
}

int com_write_data(const uint8_t *buf, size_t size)
{
    int ret;

    com_data->dev->ops.write(com_data->dev, 0, buf, size);
    ret = sem_get_timeout(com_data->write_sem, sec_to_tick(2));
    if (ret != 0) {
        //pr_err("winusb write data timedout\r\n");
        return -ETIMEDOUT;
    }

    return 0;
}

int com_send_ack(uint32_t val, uint8_t end)
{
    int ret;
    struct com_cmd_package cmd_ack_pkg;

    cmd_ack_pkg.type = COM_ACK_PKG;
    cmd_ack_pkg.data_type = DATA0;
    cmd_ack_pkg.index = 0;
    cmd_ack_pkg.end = end;
    cmd_ack_pkg.val = val;
    ret = com_write_data((uint8_t *)&cmd_ack_pkg, sizeof(cmd_ack_pkg));
    if (ret != 0)
        pr_err("send ack timedout\r\n");

    return ret;
}

int com_wait_ack(uint32_t *size)
{
    int ret;
    struct com_cmd_package cmd_ack_pkg;

    ret = com_read_data((uint8_t *)&cmd_ack_pkg, sizeof(cmd_ack_pkg));
    if (ret != 0) {
        pr_err("wait ack timedout\r\n");
        return ret;
    }

    if (cmd_ack_pkg.type != COM_ACK_PKG) {
        pr_err("ack package err\r\n");
        return -EINVAL;
    }
    if (size !=NULL)
        *size = cmd_ack_pkg.val;

    return 0;
}

static void com_task_entry(__maybe_unused void* parameter)
{
    int ret;
    struct com_cmd_package cmd_pkg;

    flash_dev = device_find_by_name("spi-flash");
    if (flash_dev == NULL) {
        pr_err("%s: not find spi-flash device\r\n", __func__);
        return;
    }

    while (1) {
        ret = com_read_data((uint8_t *)&cmd_pkg, sizeof(cmd_pkg));
        if (ret != 0)
            continue;
        pr_info("get com package ok, type = %d\r\n", cmd_pkg.type);
        switch(cmd_pkg.type) {
            case COM_HEART_PKG:
                com_send_ack(0, PKG_END);
                break;
            case COM_SET_CONFIG_PKG:
                pr_info("set config\r\n");
                break;
            case COM_GET_CONFIG_PKG:
                pr_info("get config\r\n");
                break;
            case COM_GET_VERSION_PKG:
                com_get_version();
                break;
            case COM_DOWNLOAD_PKG:
                com_download_img(&cmd_pkg);
                break;
            default:
                pr_err("unknown package(%x)\r\n", cmd_pkg.type);
                break;
        }
        delay_msec(10);
    }
}

int com_task_init(void)
{
    struct com_data *com;
    int ret;

    com_task = task_create("com", com_task_entry, NULL, COM_TASK_STACK_SIZE, COM_TASK_PRIO,
        COM_TASK_TICK, NULL, NULL);
    if (com_task == NULL) {
        pr_err("creat com task err\n");
        ret = -1;
        goto out;
    }

    com = wk_alloc(sizeof(*com), 0, com_task->pid);
    if (com == NULL) {
        pr_err("alloc com err\r\n");
        ret = -ENOMEM;
        goto out;
    }
    com_data = com;

    com->read_sem = sem_alloc(0, com_task->pid);
    if (com->read_sem == NULL) {
        pr_err("alloc com read sem err\r\n");
        ret = -ENOMEM;
        goto sem_err;
    }

    com->write_sem = sem_alloc(0, com_task->pid);
    if (com->write_sem == NULL) {
        pr_err("alloc com write sem err\r\n");
        ret = -ENOMEM;
        goto sem_err;
    }

    com_data->dev = device_find_by_name("winusb");
    if (com->dev == NULL) {
        pr_err("can't find winsub device\r\n");
        ret = -ENODEV;
        goto sem_err;
    }
    com_data->dev->ops.read_complete = com_read_complete;
    com_data->dev->ops.write_complete = com_write_complete;

    task_ready(com_task);
    
    return 0;
sem_err:
    wk_free((void *)com);
out:
    return ret;
}
task_init(com_task_init);
