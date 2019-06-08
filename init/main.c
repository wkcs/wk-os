#include <wk/kernel.h>
#include <wk/sch.h>
#include <wk/task.h>
#include <wk/cpu.h>
#include <wk/mm.h>
#include <init/init.h>
 
#include <board.h>

int wkos_start(void)
{
    disable_irq_save();
    cpu_init();
    board_init();
    mm_pool_init();
    log_server_init();
    pr_info("kernel start\r\n");
    sch_init();
    task_init_call();
    sch_start();
    return 0;
}
