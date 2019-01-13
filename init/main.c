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
        pr_info("hello %s\r\n", "world");
        delay_sec(1);
    }
}

void task_init(void)
{
    struct task_struct_t *main_task;

    main_task = task_create("main", main_task_entry, NULL, 512, 35, 3, NULL, NULL);
    task_ready(main_task);

}

int wkos_start(void)
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
