/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __WK_IPC_H__
#define __WK_IPC_H__

#include <wk/kernel.h>
#include <wk/task.h>
#include <wk/mutex.h>
#include <wk/sch.h>

#define CONFIG_MAX_MSG_POOL 8

typedef struct {
    void *addr;
    size_t len;
} msg_t;

typedef struct {
    msg_t msg;
    struct list_head list;
} msg_block_t;

struct msg_q {
    const char *name;
    struct task_struct_t *owner;
    struct mutex lock;
    struct list_head msg_list;
    struct list_head free_msg_list;
    struct list_head list;
};

#define msg_q_init(msg_q)  \
    do { \
        __msg_q_init(&(msg_q), #msg_q, get_current_task());  \
    } while(0)

int __msg_q_init(struct msg_q *msg_q, 
                const char *name,
                struct task_struct_t *owner);
int msg_q_send(struct msg_q *msg_q, msg_t *msg);
int msg_q_recv(struct msg_q *msg_q, msg_t *msg, uint32_t timeout);
struct msg_q *get_msg_q_byd_name(char *name);

#endif