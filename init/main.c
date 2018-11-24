#include <wk/kernel.h>
#include <wk/sch.h>
#include <wk/task.h>
#include <wk/mm.h>
#include <wk/clk.h>
#include <wk/timer.h>
#include <init/system.h>
#include <wk/mutex.h>
#include <wk/msg_queue.h>
#include <wk/err.h>

#include "led.h"
 
#include <board.h>

static void main_task_entry(void* parameter)
{
    while (1)
    {
        pr_info("main task running!\r\n");
        /*pr_info("run_tick = %d\r\n", get_run_tick());
        dump_timer();*/
        task_sleep(sec_to_tick(1));
    }
}
 
static void led0_task_entry(void* parameter)
{
    msg_t msg;
    int rc;
    uint8_t buf;
    struct msg_q msg_test;

    LED_Init();
    msg_q_init(msg_test);
 
    while (1)
    {
        rc = msg_q_recv(&msg_test, &msg, msec_to_tick(300));
        if (rc == -ETIMEDOUT) {
            pr_info("get msg timeout\r\n");
        } else if (rc < 0) {
            pr_err("get msg err\r\n");
        } else {
            buf = *((uint8_t *)(msg.addr));
            LED0 = buf;
            if (buf == 0)
                pr_info("led0 off\r\n");
            else
                pr_info("led0 on\r\n");
        }
    }
}

static void led1_task_entry(void* parameter)
{
    msg_t msg;
    uint8_t buf = 0;
    int rc;
    struct msg_q *msg_q;

    LED_Init();

    msg_q = get_msg_q_byd_name("msg_test");

    msg.addr = (void *)&buf;
    msg.len = 1;

    while (1)
    {
        LED1 = 1;
        pr_info("led1 on\r\n");
        buf = 0;
        rc = msg_q_send(msg_q, &msg);
        if (rc < 0)
            pr_err("msg send err (rc = %d)\r\n", rc);
        task_sleep(msec_to_tick(500));
        LED1 = 0;
        pr_info("led1 off\r\n");
        buf = 1;
        rc = msg_q_send(msg_q, &msg);
        if (rc < 0)
            pr_err("msg send err (rc = %d)\r\n", rc);
        task_sleep(msec_to_tick(500));
    }
}

void task_init(void)
{
    struct task_struct_t *main_task, *task1, *task2;

    main_task = task_create("main", main_task_entry, NULL, 512, 35, 3, NULL, NULL);
    task_ready(main_task);

    task1 = task_create("led0", led0_task_entry, NULL, 512, 10, 3, NULL, NULL);
    task_ready(task1);

    task2 = task_create("led1", led1_task_entry, NULL, 512, 20, 3, NULL, NULL);
    task_ready(task2);
}

int main(void)
{
    disable_irq_save();
    board_init();
    mm_pool_init();
    log_server_init();
    pr_info("kernel start\r\n");
    sch_init();
    system_init();
    task_init();
    sch_start();

    return 0;
}
