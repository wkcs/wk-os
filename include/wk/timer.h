/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <wk/kernel.h>
#include <wk/list.h>
#include <wk/pid.h>

struct timer_struct_t {
    char name[WK_NAME_MAX];
    uint32_t init_tick;
    uint32_t timeout_tick;
    void (*timeout_func)(void *parameter);
    void *parameter;
    uint8_t priority;
    struct list_head list;
};

enum timer_cmd_t {
    CMD_TIMER_SET_TICK = 0x00,
    CMD_TIMER_MAX,
};

int timer_init(struct timer_struct_t *timer,
                const char *name,
                void (*timeout)(void *parameter),
                void *parameter,
                uint8_t priority,
                uint32_t tick);
void timer_remove(struct timer_struct_t *timer);
struct timer_struct_t *timer_create(const char *name,
                void (*timeout)(void *parameter),
                void *parameter,
                uint8_t priority,
                uint32_t tick,
                wk_pid_t pid);
int timer_delete(struct timer_struct_t *timer);
int timer_start(struct timer_struct_t *timer);
int timer_stop(struct timer_struct_t *timer);
int timer_ctrl(struct timer_struct_t *timer, enum timer_cmd_t cmd, void *argc);
void timer_check(void);
void dump_timer(void);

#endif