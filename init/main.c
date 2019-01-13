#include <wk/kernel.h>
#include <wk/sch.h>
#include <wk/task.h>
#include <wk/mm.h>
#include <wk/clk.h>
#include <wk/timer.h>
#include <init/system.h>
#include <wk/mutex.h>
#include <wk/msg_queue.h>
#include <wk/delay.h>
#include <wk/err.h>

#include <lib/string.h>
#include <lib/vsprintf.h>

#include "led.h"
#include "lcd.h"
 
#include <board.h>

static void main_task_entry(__maybe_unused void* parameter)
{
    while (1)
    {
        //pr_info("main task running!\r\n");
        /*pr_info("run_tick = %d\r\n", get_run_tick());
        dump_timer();*/
        task_sleep(sec_to_tick(1));
    }
}
 
static void led0_task_entry(__maybe_unused void* parameter)
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

static void led1_task_entry(__maybe_unused void* parameter)
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

static void task4_entry(__maybe_unused void* parameter)
{
    msg_t msg;
    struct msg_q msg_task4;

    msg_q_init(msg_task4);
    while (1)
    {
        if (msg_q_recv(&msg_task4, &msg, msec_to_tick(300)) == 0)
            pr_info("task4 msg is %s\r\n", (char *)msg.addr);
        else
            pr_info("task4 get msg err\r\n");
    }
}

static void task5_entry(__maybe_unused void* parameter)
{
    msg_t msg;
    struct msg_q *msg_q;
    char *buf = "task5 msg";
    
    msg_q = get_msg_q_byd_name("msg_task4");

    msg.addr = (void *)buf;
    msg.len = strlen(buf);
    while (1)
    {
        msg_q_send(msg_q, &msg);

        task_sleep(sec_to_tick(1));
    }
}

static void task6_entry(__maybe_unused void* parameter)
{
    char buf[18];
    LCD_ShowString(30,150,72,9,16, "run time:");
    while (1)
    {
        sprintf(buf, "%06d.%06d msec", tick_to_sec(get_run_tick()), (tick_to_msec(get_run_tick()) % 1000) * 1000);
        LCD_ShowString(105,150,144,18,16, buf);
        //pr_info("task6 running!\r\n");
        task_sleep(msec_to_tick(500));
    }
}

static void task7_entry(__maybe_unused void* parameter)
{
    while (1)
    {
        //pr_info("task7 running!\r\n");
        task_sleep(msec_to_tick(500));
    }
}

static void task8_entry(__maybe_unused void* parameter)
{
    while (1)
    {
        //pr_info("task8 running!\r\n");
        task_sleep(msec_to_tick(500));
    }
}

static void task9_entry(__maybe_unused void* parameter)
{
    while (1)
    {
        //pr_info("task9 running!\r\n");
        task_sleep(msec_to_tick(500));
    }
}

static void task10_entry(__maybe_unused void* parameter)
{
    while (1)
    {
        //pr_info("task10 running!\r\n");
        task_sleep(msec_to_tick(500));
    }
}

static void task11_entry(__maybe_unused void* parameter)
{
    while (1)
    {
        //pr_info("task11 running!\r\n");
        task_sleep(msec_to_tick(500));
    }
}

static void task12_entry(__maybe_unused void* parameter)
{
    while (1)
    {
        //pr_info("task12 running!\r\n");
        task_sleep(msec_to_tick(500));
    }
}

static void task13_entry(__maybe_unused void* parameter)
{
    while (1)
    {
        //pr_info("task13 running!\r\n");
        task_sleep(msec_to_tick(500));
    }
}

static void task14_entry(__maybe_unused void* parameter)
{
    while (1)
    {
        //pr_info("task14 running!\r\n");
        task_sleep(msec_to_tick(500));
    }
}

static void task15_entry(__maybe_unused void* parameter)
{
    while (1)
    {
        //pr_info("task15 running!\r\n");
        task_sleep(msec_to_tick(500));
    }
}

static void task16_entry(__maybe_unused void* parameter)
{
    while (1)
    {
        //dump_all_task();
        task_sleep(sec_to_tick(5));
    }
}

static void task3_entry(__maybe_unused void* parameter)
{
    struct task_struct_t *task4, *task5, *task6, *task7, *task8, *task9, *task10, *task11, *task12, *task13, *task14, *task15, *task16;
    task4 = task_create("task4", task4_entry, NULL, 512, 14, 3, NULL, NULL);
    task_ready(task4);

    task5 = task_create("task5", task5_entry, NULL, 512, 23, 3, NULL, NULL);
    task_ready(task5);

    task6 = task_create("task6", task6_entry, NULL, 512, 64, 30, NULL, NULL);
    task_ready(task6);

    task7 = task_create("task7", task7_entry, NULL, 512, 66, 3, NULL, NULL);
    task_ready(task7);

    task8 = task_create("task8", task8_entry, NULL, 512, 77, 3, NULL, NULL);
    task_ready(task8);

    task9 = task_create("task9", task9_entry, NULL, 512, 20, 3, NULL, NULL);
    task_ready(task9);

    task10 = task_create("task10", task10_entry, NULL, 512, 20, 3, NULL, NULL);
    task_ready(task10);

    task11 = task_create("task11", task11_entry, NULL, 512, 10, 3, NULL, NULL);
    task_ready(task11);

    task12 = task_create("task12", task12_entry, NULL, 512, 45, 3, NULL, NULL);
    task_ready(task12);

    task13 = task_create("task13", task13_entry, NULL, 512, 145, 3, NULL, NULL);
    task_ready(task13);

    task14 = task_create("task14", task14_entry, NULL, 512, 222, 3, NULL, NULL);
    task_ready(task14);

    task15 = task_create("task15", task15_entry, NULL, 512, 135, 3, NULL, NULL);
    task_ready(task15);

    task16 = task_create("task16", task16_entry, NULL, 512, 67, 3, NULL, NULL);
    task_ready(task16);

    task_del(get_current_task());

    switch_task();
}

void task_init(void)
{
    struct task_struct_t *main_task, *task1, *task2, *task3;

    main_task = task_create("main", main_task_entry, NULL, 512, 35, 3, NULL, NULL);
    task_ready(main_task);

    task1 = task_create("led0", led0_task_entry, NULL, 512, 10, 3, NULL, NULL);
    task_ready(task1);

    task2 = task_create("led1", led1_task_entry, NULL, 512, 20, 3, NULL, NULL);
    task_ready(task2);

    task3 = task_create("task3", task3_entry, NULL, 512, 1, 3, NULL, NULL);
    task_ready(task3);
}

int wkos_start(void)
{
	char lcd_id[12];

    disable_irq_save();
    board_init();
    mm_pool_init();
    log_server_init();
    pr_info("kernel start\r\n");
    sch_init();
    system_init();
    task_init();
    LED_Init();
    LCD_Init();
    LCD_Clear(WHITE);
    POINT_COLOR=RED;
    sprintf((char*)lcd_id, "LCD ID:%04X", lcddev.id);
    LCD_ShowString(30, 40, 210, 24, 24, "WarShip STM32 ^_^"); 
	LCD_ShowString(30, 70, 200, 16, 16, "TFTLCD TEST");
	LCD_ShowString(30, 90, 200, 16, 16, "ATOM@ALIENTEK");
 	LCD_ShowString(30, 110, 200, 16, 16, lcd_id);
	LCD_ShowString(30, 130, 200, 12, 12, "2014/5/4");
    sch_start();
    return 0;
}
