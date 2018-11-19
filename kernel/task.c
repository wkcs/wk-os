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

extern struct list_head ready_task_list[MAX_PRIORITY];

void task_exit(void)
{
    while(1);
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
    task->offset_mask = 1L << task->offset;
    task->prio_mask = 1L << (priority & 0x07);
#elif
    task->prio_mask = 1L << priority;
#endif

    task->init_tick      = tick;
    task->remaining_tick = tick;

    task->status  = TASK_SUSPEND;

    task->cleanup   = clean;
    task->resource = resource;

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
    pr_info("stack_start = 0x%x\r\n");
    if (!stack_start)
        goto stack_err;

    pid = pid_alloc((addr_t)stack_start);
    if (!pid)
        goto pid_err;

    task = (struct task_struct_t *)wk_alloc(sizeof(struct task_struct_t), 0, pid);
    pr_info("task struct addr = 0x%x\r\n", task);
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
        pr_err("%s[%d]:task struct addr is NULL\r\n");
        return;
    }
    add_task_to_ready_list(task);
    if (get_current_task())
        switch_task();
}

int task_yield_cpu(void)
{
    register addr_t level;
    struct task_struct_t *task;

    /* disable interrupt */
    level = disable_irq_save();

    /* set to current task */
    task = get_current_task();

    if (task->status == TASK_READY && !list_empty(&task->list)) {
        /* remove task from task list */
        list_del(&(task->list));

        list_add_tail(&(task->list), &(ready_task_list[task->current_priority]));

        /* enable interrupt */
        enable_irq_save(level);

        switch_task();

        return 0;
    }

    /* enable interrupt */
    enable_irq_save(level);

    return 0;
}