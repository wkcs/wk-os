/*
 * Copyright (C) 2019 胡启航 <Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __WK_INPUT_H__
#define __WK_INPUT_H__

#include <wk/kernel.h>
#include <wk/list.h>
#include <wk/sem.h>

#define EVENT_NUM_MAX U8_MAX

struct key_event {
    uint8_t code;
    uint32_t type;
    struct list_head list;
};

struct input_dev {
    char *name;
    sem_t *input_sem;
    struct list_head event_list;
    struct list_head listen_list;
    struct list_head list;
    uint8_t event_num;
};

#endif