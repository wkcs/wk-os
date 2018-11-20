#include <wk/kernel.h>
#include <wk/sch.h>
#include <wk/task.h>
#include <wk/mm.h>
#include <wk/clk.h>
#include <wk/timer.h>
#include <init/system.h>
#include "led.h"
 
#include <board.h>

static void main_task_entry(void* parameter)
{
    while (1)
    {
        pr_info("main task running!\r\n");
        pr_info("run_tick = %d\r\n", get_run_tick());
        dump_timer();
        task_sleep(1000);
    }
}
 
static void led1_task_entry(void* parameter)
{
     LED_Init();
 

    while (1)
    {
        LED0 = 1;
        pr_info("led0 on\r\n");
        task_sleep(1000);
        LED0 = 0;
        pr_info("led0 off\r\n");
        task_sleep(1000);
     }
}

static void led2_task_entry(void* parameter)
{
    LED_Init();

    while (1)
    {
        LED1 = 1;
        pr_info("led1 on\r\n");
        task_sleep(500);
        LED1 = 0;
        pr_info("led1 off\r\n");
        task_sleep(500);
    }
}

void task_init(void)
{
    struct task_struct_t *main_task, *task1, *task2;

    main_task = task_create("main", main_task_entry, NULL, 256, 35, 3, NULL, NULL);
    task_ready(main_task);

    task1 = task_create("led1", led1_task_entry, NULL, 256, 10, 3, NULL, NULL);
    task_ready(task1);

    task2 = task_create("led2", led2_task_entry, NULL, 256, 10, 3, NULL, NULL);
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
