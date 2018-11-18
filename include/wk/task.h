/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __TASK_H__
#define __TASK_H__

#include <wk/kernel.h>
#include <wk/pid.h>
#include <wk/list.h>

enum task_status_t {
    TASK_RUNING = 0x01,
    TASK_READY = 0x02,
    TASK_WAIT = 0x04,
    TASK_SUSPEND = 0x08,
    TASK_CLOSE = 0x10
};

struct task_struct_t {
    addr_t *sp;
    
    char name[TASK_NAME_MAX];
    wk_pid_t pid;

    addr_t *entry;                                
    addr_t *parameter;                             
    addr_t *stack_addr;                           
    size_t stack_size;
    addr_t *resource;

    uint32_t  init_tick;      
    uint32_t  remaining_tick;

    void (*cleanup)(struct task_struct_t *task);

    uint8_t init_priority;
    uint8_t current_priority;
    uint8_t status;

#if MAX_PRIORITY > 32
    uint8_t  offset;
    uint8_t  offset_mask;
#endif
    uint32_t prio_mask;

    struct list_head list;                           
};
struct task_struct_t * task_create(const char *name,
                                    void (*entry)(void *parameter),
                                    void *parameter,
                                    size_t stack_size,
                                    uint8_t priority,
                                    uint32_t tick,
                                    void (*clean)(struct task_struct_t *task),
                                    addr_t *resource);
int task_yield_cpu(void);

#endif