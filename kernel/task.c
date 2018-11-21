/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/task.h>
#include <wk/sch.h>
#include <lib/string.h>
#include <wk/irq.h>
#include <wk/cpu.h>
#include <wk/mm.h>
#include <wk/list.h>
#include <wk/timer.h>

extern struct list_head ready_task_list[MAX_PRIORITY];

static void task_exit(void)
{
    register addr_t level;
    struct task_struct_t *task;
    
    task = get_current_task();

    WK_ERROR(task != NULL);

    level = disable_irq_save();

    del_task_to_ready_list(task);
    task->status = TASK_CLOSE;
    timer_delete(&task->timer);

    enable_irq_save(level);

    switch_task();
}

static void timeout(void *parameter)
{
    register addr_t level;
    struct task_struct_t *task;

    task = (struct task_struct_t *)parameter;

    WK_ERROR(task != NULL);
    WK_ERROR(task->status == TASK_WAIT);

    level = disable_irq_save();

    list_del(&task->list);

    enable_irq_save(level);
    
    add_task_to_ready_list(task);

    switch_task();
}

int __task_create(struct task_struct_t *task,
                const char *name,
                void (*entry)(void *parameter),
                void *parameter,
                addr_t *stack_start,
                size_t stack_size,
                uint8_t priority,
                uint32_t tick,
                void (*clean)(struct task_struct_t *task),
                addr_t *resource)
{
    size_t len;

    INIT_LIST_HEAD(&task->list);

    memset(task->name, 0, WK_NAME_MAX);
    len = strlen(name);
    if (len < WK_NAME_MAX)
        memcpy(task->name, name, len);
    else
        memcpy(task->name, name, WK_NAME_MAX - 1);

    task->entry = (void *)entry;
    task->parameter = parameter;

    task->stack_addr = stack_start;
    task->stack_size = stack_size;

    task->sp = (addr_t *)stack_init(task->entry, task->parameter,
                                        (addr_t *)task->stack_addr,
                                        (void *)task_exit);

    task->init_priority    = priority;
    task->current_priority = priority;

#if MAX_PRIORITY > 32
    task->offset = priority >> 3;
    task->offset_mask = 1UL << task->offset;
    task->prio_mask = 1UL << (priority & 0x07);
#elif
    task->offset_mask = 1UL << priority;
#endif

    task->init_tick      = tick;
    task->remaining_tick = tick;

    task->status  = TASK_SUSPEND;

    task->cleanup   = clean;
    task->resource = resource;

    timer_init(&task->timer, task->name, timeout, task, priority, 0);

    return 0;
}

struct task_struct_t * task_create(const char *name,
                                    void (*entry)(void *parameter),
                                    void *parameter,
                                    size_t stack_size,
                                    uint8_t priority,
                                    uint32_t tick,
                                    void (*clean)(struct task_struct_t *task),
                                    addr_t *resource)
{
    struct task_struct_t *task;
    addr_t *stack_start;
    wk_pid_t pid;

    stack_start = (addr_t *)stack_alloc(stack_size);
    if (!stack_start)
        goto stack_err;

    pid = pid_alloc((addr_t)stack_start);
    if (!pid)
        goto pid_err;

    task = (struct task_struct_t *)wk_alloc(sizeof(struct task_struct_t), 0, pid);
    if (!task)
        goto pid_err;

    task->pid = pid;
    
    if (__task_create(task, name, entry, parameter, stack_start, 
                        stack_size, priority, tick, clean, resource))
        goto task_create_err;
    
    return task;

task_create_err:
    wk_free(task);
pid_err:
stack_err:
    stack_free(stack_size, stack_start);
    return NULL;
}

void task_ready(struct task_struct_t *task)
{
    if (!task) {
        pr_err("%s[%d]:task struct is NULL\r\n", __func__, __LINE__);
        return;
    }

    if (task->status == TASK_READY || task->status == TASK_RUNING) {
        pr_err("%s[%d]:task is ready\r\n", __func__, __LINE__);
        return;
    }

    add_task_to_ready_list(task);
    if (get_current_task())
        switch_task();
}

int task_hang(struct task_struct_t *task)
{
    register addr_t level;

    if (!task) {
        pr_err("%s[%d]:task struct is NULL\r\n", __func__, __LINE__);
        return -1;
    }

    if (task->status != TASK_READY && task->status != TASK_RUNING) {
        pr_err("%s[%d]:task status is not TASK_READY or TASK_RUNING\r\n", __func__, __LINE__);
        return -1;
    }

    level = disable_irq_save();

    task->status = TASK_WAIT;

    del_task_to_ready_list(task);
    timer_stop(&task->timer);

    enable_irq_save(level);

    return 0;
}

int task_resume(struct task_struct_t *task)
{
    register addr_t level;

    if (!task) {
        pr_err("%s[%d]:task struct is NULL\r\n", __func__, __LINE__);
        return -1;
    }

    if (task->status != TASK_WAIT) {
        pr_err("%s[%d]:task status is not TASK_WAIT\r\n", __func__, __LINE__);
        return -1;
    }

    level = disable_irq_save();

    list_del(&task->list);
    timer_stop(&task->timer);

    enable_irq_save(level);

    add_task_to_ready_list(task);

    return 0;
}

int task_sleep(uint32_t tick)
{
    register addr_t level;
    struct task_struct_t *task;
    int rc = 0;

    if (!tick) {
        pr_err("%s[%d]:sleep tick is 0\r\n", __func__, __LINE__);
        return -1;
    }

    task = get_current_task();

    if (!task) {
        pr_err("%s[%d]:task struct is NULL\r\n", __func__, __LINE__);
        return -1;
    }

    level = disable_irq_save();

    rc = task_hang(task);
    if (rc) {
        pr_err("%s[%d]:task hang err(%d)\r\n", __func__, __LINE__, rc);
        return rc;
    }
    timer_ctrl(&task->timer, CMD_TIMER_SET_TICK, &tick);
    timer_start(&task->timer);

    enable_irq_save(level);

    switch_task();

    return 0;
}

int task_yield_cpu(void)
{
    register addr_t level;
    struct task_struct_t *task;

    /* disable interrupt */
    level = disable_irq_save();

    /* set to current task */
    task = get_current_task();

    if (task->status == TASK_RUNING && !list_empty(&task->list)) {
        /* remove task from task list */
        list_del(&(task->list));

        list_add_tail(&(task->list), &(ready_task_list[task->current_priority]));

        task->status = TASK_READY;
        /* enable interrupt */
        enable_irq_save(level);

        switch_task();

        return 0;
    }

    /* enable interrupt */
    enable_irq_save(level);

    return 0;
}

static inline int task_set_prio(struct task_struct_t *task, uint32_t prio)
{
    register addr_t level;

    if (task == NULL) {
        pr_err("%s[%d]:task struct is NULL\r\n", __func__, __LINE__);
        return -1;
    }

    level = disable_irq_save();

    del_task_to_ready_list(task);
    task->current_priority = prio;
    add_task_to_ready_list(task);

    enable_irq_save(level);

    return 0;
}

int task_ctrl(struct task_struct_t *task, enum task_cmd_t cmd, void *argc)
{
    if (task == NULL) {
        pr_err("%s[%d]:task struct is NULL\r\n", __func__, __LINE__);
        return -1;
    }

    switch (cmd) {
        case CMD_TASK_SET_CURR_PRIO:
            task_set_prio(task, *(uint32_t *)argc);
            break;
        case CMD_TASK_SET_INIT_PRIO:
            task->init_priority = *(uint32_t *)argc;
            break;
        case CMD_TASK_SET_INIT_TICK:
            task->init_tick = *(uint32_t *)argc;
            break;
        default:
            pr_err("%s[%d]:Without this cmd(%d)\r\n", __func__, __LINE__, cmd);
            return -1;
    }

    return 0;
}

void dump_all_task(void)
{
    int i;
    struct task_struct_t *task_temp;

    for (i = 0; i < MAX_PRIORITY; i++) {
        list_for_each_entry(task_temp, &ready_task_list[i], list) {
            pr_info("task[%s]:prio=%d, pid=%d\r\n", task_temp->name, task_temp->current_priority, task_temp->pid);
        }
    }
}
