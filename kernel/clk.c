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

static uint32_t run_time;

void system_beat_processing(void)
{
    struct task_struct_t *task;

    run_time++;

    task = get_current_task();

    task->remaining_tick--;
    if (task->remaining_tick == 0)
    {
        /* change to initialized tick */
        task->remaining_tick = task->init_tick;

        /* yield */
        task_yield_cpu();
    }
}