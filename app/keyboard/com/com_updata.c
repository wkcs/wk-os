#include <wk/err.h>
#include <wk/mm.h>
#include <wk/sch.h>
#include <wk/delay.h>
#include <wk/device.h>
#include <wk/cpu.h>
#include "com.h"

#define IMG_MAGIC 0x20302030

#define BOOTLOADER_INDEX 0
#define KERNEL_A_INDEX   1
#define KERNEL_B_INDEX   2

#define BOOTLOADER_ADDR 0x0
#define KERNEL_A_ADDR   0x8000
#define KERNEL_B_ADDR   0x3A000

#define REBOOT_FLASH_BASE 0x7f00000

struct img_head_info {
    uint32_t magic;
    uint32_t check_sum;
    uint32_t size;
    uint8_t type;
    uint8_t version[3];
} __attribute__ ((packed));

int write_img_to_flash(uint8_t *img_buf, uint32_t size, uint32_t addr_index, uint8_t img_index)
{
    addr_t addr;

    switch (img_index) {
        case BOOTLOADER_INDEX:
            addr = BOOTLOADER_INDEX;
            break;
        case KERNEL_A_INDEX:
            addr = KERNEL_A_ADDR;
            break;
        case KERNEL_B_INDEX:
            addr = KERNEL_B_ADDR;
            break;
        default:
            pr_err("Wrong partition number(%d)\r\n", img_index);
            return -EINVAL;
    }
    addr += addr_index;
    if (flash_dev->ops.write == NULL) {
        pr_err("%s: %s don't susport write\r\n", __func__, flash_dev->name);
        return -EIO;
    }
    flash_dev->ops.write(flash_dev, addr, (void *)img_buf, size);

    return 0;
}

void write_reboot_flag(uint8_t reboot_flag)
{
    flash_dev->ops.write(flash_dev, REBOOT_FLASH_BASE, (void *)&reboot_flag, 1);
}

int com_download_img(struct com_cmd_package *cmd_pkg)
{
    struct com_cmd_package pkg_tmp;
    uint8_t *img_buf;
    uint32_t size;
    uint32_t total_size = 0;
    addr_t addr = 0;
    uint8_t end = PKG_END;
    uint8_t data_type;
    uint8_t last_data_type = DATA_ERR;
    uint8_t img_index;
    int ret;

    size = cmd_pkg->val;
    pr_info("download img start, size = %d\r\n", size);
    img_buf = wk_alloc(size, 0, get_current_task()->pid);
    if (img_buf == NULL) {
        pr_err("alloc img buf err\r\n");
        return -ENOMEM;
    }
    end = cmd_pkg->end;
    data_type = cmd_pkg->data_type;
    img_index = cmd_pkg->index;

    while (1) {
        ret = com_send_ack(0, PKG_END);
        if (ret != 0)
            goto err;
        ret = com_read_data(img_buf, size);
        if (ret != 0)
            goto err;
        write_img_to_flash(img_buf, size, addr, img_index);
        if (data_type != last_data_type) {
            addr += size;
            total_size += size;
            last_data_type = data_type;
        }
        pr_info("write img(size=%d) to 0x%08x\r\n", size, addr);
        ret = com_send_ack(0, PKG_END);
        if (ret != 0)
            goto err;
        if (end != PKG_NOT_END) {
            pr_info("download img ok, total size = %d\r\n", total_size);
            break;
        }
        delay_msec(10);
        ret = com_read_data((uint8_t *)&pkg_tmp, sizeof(pkg_tmp));
        if (ret != 0 || pkg_tmp.type != COM_DOWNLOAD_PKG)
            goto err;
        end = pkg_tmp.end;
        size = pkg_tmp.val;
        data_type = pkg_tmp.data_type;
        if (img_index != pkg_tmp.index)
            goto err;
    }

    pr_info("download img success, reboot now\r\n");
    write_reboot_flag(img_index);
    cpu_reboot();
    goto out;
err:
    pr_err("download img err, ret = %d\r\n", ret);
out:
    wk_free((void *)img_buf);
    return ret;
}
