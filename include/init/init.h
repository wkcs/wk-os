/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __INIT_INIT_H__
#define __INIT_INTI_H__

#include <wk/section.h>

typedef struct {
    char *name;
    int (*task_init_func)(void);
} init_task_t;

#define task_init(fn) \
    __visible __init_task const init_task_t __maybe_unused __int_##fn = { \
            .name = #fn, \
            .task_init_func = fn \
        }

void task_init_call(void);

#endif