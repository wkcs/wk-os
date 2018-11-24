/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <wk/kernel.h>
#include <wk/list.h>

#define CONFIG_DEBUG_MUTEXES

struct mutex {
    int count;
    struct list_head wait_list;
    struct task_struct_t *owner;
#ifdef CONFIG_DEBUG_MUTEXES
    const char *name;
#endif
};

#define debug_mutex_init(lock, name) do {} while (0)

#define mutex_init(mutex)  \
    do {  \
        __mutex_init(&(mutex), #mutex);  \
    } while (0)

void __mutex_init(struct mutex *lock, const char *name);
void mutex_lock(struct mutex *lock);
int mutex_try_lock(struct mutex *lock);
void mutex_unlock(struct mutex *lock);

#endif
