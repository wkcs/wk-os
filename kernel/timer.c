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

int timer_init(struct timer_struct_t *timer,
                const char *name,
                void (*timeout)(void *parameter),
                void *parameter,
                uint8_t priority,
                uint32_t tick)
{
    size_t len;

    if (tick > TIMER_TICK_MAX) {
        pr_err("%s[%d]:tick(=%u) > TIMER_TICK_MAX\r\n", __func__, __LINE__, tick);
        return -1;
    }

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

    return 0;
}

void timer_remove(struct timer_struct_t *timer)
{
    list_del(&timer->list);
}

inline uint32_t get_lave_tick(struct timer_struct_t *timer)
{
    uint32_t run_tick = get_run_tick();
    uint32_t lave_tick = 0;

    if (timer->timeout_tick >= run_tick)
        return timer->timeout_tick - run_tick;
    else {
        lave_tick = WK_U32_MAX - run_tick + timer->timeout_tick;
        if (lave_tick > TIMER_TICK_MAX)
            return 0;
        else
            return lave_tick;
    }
}

struct timer_struct_t *timer_create(const char *name,
                void (*timeout)(void *parameter),
                void *parameter,
                uint8_t priority,
                uint32_t tick,
                wk_pid_t pid)
{
    struct timer_struct_t *timer;
    int rc = 0;

    if (tick > TIMER_TICK_MAX) {
        pr_err("%s[%d]:tick(=%u) > TIMER_TICK_MAX\r\n", __func__, __LINE__, tick);
        return NULL;
    }

    timer = (struct timer_struct_t *)wk_alloc(sizeof(struct timer_struct_t), 0, pid);
    if (!timer) {
        pr_err("%s[%d]:alloc timer struct filed\r\n");
        return timer;
    }

    rc = timer_init(timer, name, timeout, parameter, priority, tick);

    if (rc) {
        pr_err("%s[%d]:timer init err(rc = %d)\r\n", __func__, __LINE__, rc);
        return NULL;
    }

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
    WK_ERROR(timer->init_tick != 0);

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

int timer_ctrl(struct timer_struct_t *timer, enum timer_cmd_t cmd, void *argc)
{
    if (timer == NULL) {
        pr_err("%s[%d]:timer struct is NULL\r\n", __func__, __LINE__);
        return -1;
    }

    switch (cmd) {
        case CMD_TIMER_SET_TICK:
            timer->init_tick = *(uint32_t *)argc;
            break;
        default:
            pr_err("%s[%d]:Without this cmd(%d)\r\n", __func__, __LINE__, cmd);
            return -1;
    }

    return 0;
}


void timer_check(void)
{
    register addr_t level;
    struct timer_struct_t *timer;

    level = disable_irq_save();

    if (!list_empty(&system_timer_list)) {
        timer = list_entry(system_timer_list.next, struct timer_struct_t, list);
        if (!get_lave_tick(timer)) {
            timer_remove(timer);
            timer->timeout_func(timer->parameter);
        }
    }

    enable_irq_save(level);
}

void dump_timer(void)
{
    struct timer_struct_t *list_temp;

    list_for_each_entry(list_temp, &system_timer_list, list) {
        pr_info("timer[%s]:timeout = %u, lave = %u\r\n", list_temp->name, list_temp->timeout_tick, get_lave_tick(list_temp));
    }
}

