 #include <lib/stdio.h>
 #include <wk/printk.h>
 #include <wk/log.h>
#include <wk/sch.h>
#include <wk/task.h>
#include "led.h"
 
 #include <board.h>
 
static void led1_task_entry(void* parameter)
{
    uint32_t count=0;

     LED_Init();
 

    while (1)
    {
        LED0 = 1;
        pr_info("led0 on\r\n");
        while(count < 1000000)
            count++;
        count = 0;
        LED0 = 0;
        pr_info("led0 off\r\n");
        while(count < 1000000)
            count++;
        count = 0;
     }
}

static void led2_task_entry(void* parameter)
{
    uint32_t count=0;

    LED_Init();

    while (1)
    {
        LED1 = 1;
        pr_info("led1 on\r\n");
        while(count < 1000000)
            count++;
        count = 0;
        LED1 = 0;
        pr_info("led1 off\r\n");
        while(count < 1000000)
            count++;
        count = 0;
    }
}

void task_init(void)
{
    struct task_struct_t *task1, *task2;

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
    task_init();
    sch_start();
}