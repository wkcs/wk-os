/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/timer.h>
#include <wk/task.h>
#include <wk/sch.h>
#include <wk/irq.h>
#include <wk/clk.h>
#include <wk/mm.h>
#include <lib/string.h>

static LIST_HEAD(system_timer_list);

void timer_init(struct timer_struct_t *timer,
                const char *name,
                void (*timeout)(void *parameter),
                void *parameter,
                uint8_t priority,
                uint32_t tick)
{
    size_t len;

    INIT_LIST_HEAD(&timer->list);

    memset(timer->name, 0, WK_NAME_MAX);
    len = strlen(name);
    if (len < WK_NAME_MAX)
        memcpy(timer->name, name, len);
    else
        memcpy(timer->name, name, WK_NAME_MAX - 1);

    timer->timeout_func = timeout;
    timer->parameter = parameter;
    timer->priority = priority;
    timer->init_tick = tick;
}

void timer_remove(struct timer_struct_t *timer)
{
    list_del(&timer->list);
}

inline uint32_t get_lave_tick(struct timer_struct_t *timer)
{
    uint32_t run_tick = get_run_tick();

    if (timer->timeout_tick >= run_tick)
        return timer->timeout_tick - run_tick;
    else
        return WK_U32_MAX - run_tick + timer->timeout_tick;
}

struct timer_struct_t *timer_create(const char *name,
                void (*timeout)(void *parameter),
                void *parameter,
                uint8_t priority,
                uint32_t tick,
                wk_pid_t pid)
{
    struct timer_struct_t *timer;

    timer = (struct timer_struct_t *)wk_alloc(sizeof(struct timer_struct_t), 0, pid);
    if (!timer) {
        pr_err("%s[%d]:alloc timer struct filed\r\n");
        return timer;
    }

    timer_init(timer, name, timeout, parameter, priority, tick);

    return timer;
}

int timer_delete(struct timer_struct_t *timer)
{
    register addr_t level;

    WK_ERROR(timer != NULL);

    level = disable_irq_save();

    timer_remove(timer);

    enable_irq_save(level);

    return 0;
}

int timer_start(struct timer_struct_t *timer)
{
    register addr_t level;
    struct timer_struct_t *list_temp;

    WK_ERROR(timer != NULL);

    level = disable_irq_save();

    timer_remove(timer);

    if (WK_U32_MAX - get_run_tick() >= timer->init_tick)
        timer->timeout_tick = get_run_tick() + timer->init_tick;
    else
        timer->timeout_tick = timer->init_tick - (WK_U32_MAX - get_run_tick());

    list_for_each_entry(list_temp, &system_timer_list, list) {
        uint32_t tick_list = get_lave_tick(list_temp);
        uint32_t tick_new = get_lave_tick(timer);
        if ((tick_list < tick_new) || (tick_list == tick_new && list_temp-> priority <= timer->priority))
            continue;
        else
            break;
    }

    list_add_tail(&timer->list, &list_temp->list);

    enable_irq_save(level);

    return 0;
}

int timer_stop(struct timer_struct_t *timer)
{
    register addr_t level;

    WK_ERROR(timer != NULL);

    level = disable_irq_save();

    timer_remove(timer);

    enable_irq_save(level);

    return 0;
}


void timer_check(void)
{
    register addr_t level;
    struct timer_struct_t *timer;

    level = disable_irq_save();

    if (!list_empty(&system_timer_list)) {
        timer = list_entry(&system_timer_list, struct timer_struct_t, list);
        if (timer->timeout_tick == get_run_tick()) {
            timer_remove(timer);
            timer->timeout_func(timer->parameter);
        }
    }

    enable_irq_save(level);
}

