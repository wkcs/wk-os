/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/clk.h>
#include <wk/task.h>
#include <wk/list.h>
#include <wk/sch.h>

/*内核当前已经运行的时间片，最大值为2^32*/
static uint32_t run_tick;

/*
 * 
 */
void system_beat_processing(void)
{
    struct task_struct_t *task;

    if (run_tick < WK_U32_MAX)
        run_tick++;
    else
        run_tick = 1;

    task = get_current_task();

    task->remaining_tick--;
    if (task->remaining_tick == 0)
    {
        /* change to initialized tick */
        task->remaining_tick = task->init_tick;

        /* yield */
        task_yield_cpu();
    }

    timer_check();
}

/*
 * 获取kernel当前运行时间片
 */
uint32_t get_run_tick(void)
{
    return run_tick;
}
