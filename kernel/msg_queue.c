/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/msg_queue.h>
#include <wk/mm.h>
#include <wk/err.h>
#include <lib/string.h>

static LIST_HEAD(msg_q_list);

static int msg_pool_init(struct msg_q *msg_q, void *addr, size_t num)
{
    size_t i;
    msg_block_t *msg_list_temp;

    if (addr == NULL) {
        pr_err("%s[%d]:addr is NULL\r\n", __func__, __LINE__);
        return -1;
    }

    if (num == 0) {
        pr_err("%s[%d]:num is zero\r\n", __func__, __LINE__);
        return -1;
    }

    for (i = 0; i < num; i++) {
        msg_list_temp = (msg_block_t *)((addr_t)addr + i * sizeof(msg_block_t));
        list_add_tail(&msg_list_temp->list, &msg_q->free_msg_list);
    }
    return 0;
}

int __msg_q_init(struct msg_q *msg_q, 
                const char *name,
                struct task_struct_t *owner)
{
    void *addr;
    int rc;

    if (msg_q == NULL) {
        pr_err("%s[%d]:msg_q is NULL\r\n", __func__, __LINE__);
        return -1;
    }

    if (owner == NULL) {
        pr_err("%s[%d]:owner is NULL\r\n", __func__, __LINE__);
        return -1;
    }

    __mutex_init(&msg_q->lock, name);

    msg_q->name = name;
    msg_q->owner = owner;

    INIT_LIST_HEAD(&msg_q->msg_list);
    INIT_LIST_HEAD(&msg_q->free_msg_list);
    INIT_LIST_HEAD(&msg_q->list);

    addr = wk_alloc(sizeof(msg_block_t) * CONFIG_MAX_MSG_POOL, 0, owner->pid);

    if (addr == NULL) {
        pr_err("%s[%d]:mem alloc err\r\n", __func__, __LINE__);
        return -1;
    }

    rc = msg_pool_init(msg_q, addr, CONFIG_MAX_MSG_POOL);

    if (rc) {
        pr_err("%s[%d]:msg_pool_init err (rc = %d)\r\n", __func__, __LINE__, rc);
        goto msg_pool_init_err;
    }

    mutex_lock(&msg_q->lock);
    list_add_tail(&msg_q->list, &msg_q_list);
    mutex_unlock(&msg_q->lock);

    return 0;

msg_pool_init_err:
    wk_free(addr);

    return -1;
}

static inline msg_block_t *mag_block_alloc(struct msg_q *msg_q)
{
    msg_block_t *msg_block;

    if (list_empty(&msg_q->free_msg_list))
        return NULL;

    msg_block = list_entry(msg_q->free_msg_list.next, msg_block_t, list);
    list_del(&msg_block->list);

    return msg_block;
}

static inline void mag_block_free(struct msg_q *msg_q, msg_block_t *msg_block)
{
    list_add_tail(&msg_block->list, &msg_q->free_msg_list);
}

int msg_q_send(struct msg_q *msg_q, msg_t *msg)
{
    msg_block_t *msg_block;
    int rc = 0;

    if (msg_q == NULL) {
        pr_err("%s[%d]:msg_q is NULL\r\n", __func__, __LINE__);
        return -EFAULT;
    }

    if (msg == NULL) {
        pr_err("%s[%d]:msg is NULL\r\n", __func__, __LINE__);
        return -EFAULT;
    }

    mutex_lock(&msg_q->lock);

    msg_block = mag_block_alloc(msg_q);

    if (msg_block == NULL) {
        pr_err("%s[%d]:msg pool is empty\r\n", __func__, __LINE__);
        rc = -EXFULL;
        goto alloc_err;
    }

    msg_block->msg.addr = msg->addr;
    msg_block->msg.len = msg->len;

    list_add_tail(&msg_block->list, &msg_q->msg_list);

    mutex_unlock(&msg_q->lock);

    if (msg_q->owner->status != TASK_READY) {
        task_resume(msg_q->owner);
        switch_task();
    }

    return 0;

alloc_err:
    mutex_unlock(&msg_q->lock);
    return rc;
}

int msg_q_recv(struct msg_q *msg_q, msg_t *msg, uint32_t timeout)
{
    msg_block_t *msg_block;
    register addr_t level;
    struct task_struct_t *task;
    int rc = 0;

    if (msg_q == NULL) {
        pr_err("%s[%d]:msg_q is NULL\r\n", __func__, __LINE__);
        return -EFAULT;
    }

    if (msg == NULL) {
        pr_err("%s[%d]:msg is NULL\r\n", __func__, __LINE__);
        return -EFAULT;
    }

    task = get_current_task();

    if (task == NULL) {
        pr_err("%s[%d]:task struct is NULL\r\n", __func__, __LINE__);
        return -EFAULT;
    }

    if (list_empty(&msg_q->msg_list)) {
        if (timeout == 0)
            return -ENOMSG;

        level = disable_irq_save();

        rc = task_hang(task);
        if (rc) {
            pr_err("%s[%d]:task hang err(%d)\r\n", __func__, __LINE__, rc);
            return rc;
        }
        timer_ctrl(&task->timer, CMD_TIMER_SET_TICK, &timeout);
        timer_start(&task->timer);

        enable_irq_save(level);

        switch_task();
    }

    if (task->flag == -ETIMEDOUT) {
        task->flag = 0;
        return -ETIMEDOUT;
    }

    /*再检查一次消息队列是否为空，防止任务被其他事件唤醒时消息队列为空*/
    if (list_empty(&msg_q->msg_list)) {
        pr_info("%s[%d]:msg is empty\r\n", __func__, __LINE__);
        return -ENOMSG;
    }

    /*从消息队列读取一条消息*/
    mutex_lock(&msg_q->lock);
    msg_block = list_entry(msg_q->msg_list.next, msg_block_t, list);
    msg->addr = msg_block->msg.addr;
    msg->len = msg_block->msg.len;
    list_del(&msg_block->list);
    mag_block_free(msg_q, msg_block);
    mutex_unlock(&msg_q->lock);

    return 0;
}

struct msg_q *get_msg_q_byd_name(char *name)
{
    struct msg_q *msg_q_temp;

    if (name == NULL) {
        pr_err("%s[%d]:name addr is NULL\r\n", __func__, __LINE__);
        return NULL;
    }

    list_for_each_entry(msg_q_temp, &msg_q_list, list) {
        if (!rt_strcmp(name, msg_q_temp->name))
            return msg_q_temp;
    }

    return NULL;
}
