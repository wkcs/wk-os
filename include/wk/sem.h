/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __WK_SEM_H__
#define __WK_SEM_H__

#include <wk/kernel.h>
#include <wk/list.h>
#include <wk/mutex.h>
#include <wk/pid.h>

typedef struct {
    uint8_t count;
    struct list_head wait_list;
#ifdef CONFIG_DEBUG_SEM
    const char *name;
#endif
} sem_t;

#define debug_sem_init(sem, name) do {} while (0)

sem_t *sem_alloc(uint8_t init_num, wk_pid_t pid);
void sem_get(sem_t *sem);
int sem_get_timeout(sem_t *sem, uint32_t timedout);
void sem_send(sem_t *sem, uint8_t num);

#define sem_send_one(sem) sem_send(sem, 1)

#endif