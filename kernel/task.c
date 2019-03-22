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
#include <wk/err.h>

extern struct list_head ready_task_list[MAX_PRIORITY];

static LIST_HEAD(task_list);
LIST_HEAD(close_task_list);

static void task_exit(void)
{
    struct task_struct_t *task;
    
    task = get_current_task();

    WK_ERROR(task != NULL);

    task_del(task);

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

    task->flag = -ETIMEDOUT;

    list_del(&task->list);

    enable_irq_save(level);

    add_task_to_ready_list(task);

    switch_task();
}

int __task_create(struct task_struct_t *task,
                const char *name,
                void (*entry)(__maybe_unused void *parameter),
                void *parameter,
                addr_t *stack_start,
                size_t stack_size,
                uint8_t priority,
                uint32_t tick,
                void (*clean)(struct task_struct_t *task),
                addr_t *resource)
{
    register addr_t level;
    size_t len;

    if (priority >= MAX_PRIORITY) {
        pr_err("%s: Priority should be less than %d\r\n", __func__, MAX_PRIORITY);
        return -EINVAL;
    }

    INIT_LIST_HEAD(&task->list);
    INIT_LIST_HEAD(&task->tlist);
    INIT_LIST_HEAD(&task->wait_list);

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
#else
    task->offset_mask = 1UL << priority;
#endif

    task->init_tick      = tick;
    task->remaining_tick = tick;

    task->status  = TASK_SUSPEND;

    task->cleanup   = clean;
    task->resource = resource;

    task->flag = 0;

    timer_init(&task->timer, task->name, timeout, task, priority, 0);

    level = disable_irq_save();
    list_add_tail(&task->tlist, &task_list);
    enable_irq_save(level);

    return 0;
}

struct task_struct_t * task_create(const char *name,
                                    void (*entry)(__maybe_unused void *parameter),
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

    if (priority >= MAX_PRIORITY) {
        pr_err("%s: Priority should be less than %d\r\n", __func__, MAX_PRIORITY);
        return NULL;
    }

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

void task_del(struct task_struct_t *task)
{
    register addr_t level;

    if (!task) {
        pr_err("%s[%d]:task struct is NULL\r\n", __func__, __LINE__);
        return;
    }

    level = disable_irq_save();

    if (task->status == TASK_READY || task->status == TASK_RUNING)
        del_task_to_ready_list(task);

    list_del(&task->tlist);
    task->status = TASK_CLOSE;
    timer_delete(&task->timer);
    list_add_tail(&task->list, &close_task_list);

    enable_irq_save(level);
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

    if(task->flag == -ETIMEDOUT)
        task->flag = 0;

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

        //task->status = TASK_READY;
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

struct task_struct_t *find_task_by_name(char *name)
{
    struct task_struct_t *task_temp;

    if (name == NULL) {
        pr_err("%s[%d]:name addr is NULL\r\n", __func__, __LINE__);
        return NULL;
    }

    list_for_each_entry(task_temp, &task_list, tlist) {
        if (!rt_strcmp(name, task_temp->name))
            return task_temp;
    }

    return NULL;
}

struct task_struct_t *find_task_by_pid(wk_pid_t pid)
{
    struct task_struct_t *task_temp;

    list_for_each_entry(task_temp, &task_list, tlist) {
        if (pid == task_temp->pid)
            return task_temp;
    }

    return NULL;
}

bool task_stack_check(struct task_struct_t *task)
{
#ifdef CONFIG_STACK_GROW_DOWN
    if (*(char *)((addr_t)task->stack_addr - task->stack_size + 4) != '*')
        return false;
    else
        return true;
#else
    if (*(char *)((addr_t)task->stack_addr + task->stack_size) != '*')
        return false;
    else
        return true;
#endif
}

size_t task_get_stack_max_used(struct task_struct_t *task)
{
    size_t used, i;
    addr_t addr;

    if (!task_stack_check(task))
        return 0;
    
    used = task->stack_size;

#ifdef CONFIG_STACK_GROW_DOWN
    addr = (addr_t)task->stack_addr - task->stack_size + 4;
    for (i = 0; i < task->stack_size; i++) {
        if (*(char *)(addr + i) == '*')
            used--;
        else
            return used;
    }
#elif
    addr = (addr_t)task->stack_addr + task->stack_size;
    for (i = 0; i < task->stack_size; i++) {
        if (*(char *)(addr - i) == '*')
            used--;
        else
            return used;
    }
#endif

    return used;
}

char *get_task_status_string(struct task_struct_t *task)
{
    if (task == NULL) {
        pr_err("%s[%d]:task struct is NULL\r\n", __func__, __LINE__);
        return NULL;
    }

    switch(task->status) {
        case TASK_RUNING:
            return "RUNING";
        case TASK_READY:
            return "READY";
        case TASK_WAIT:
            return "WAIT";
        case TASK_SUSPEND:
            return "SUSPEND";
        case TASK_CLOSE:
            return "CLOSE";
        default:
            return "UNKNOWN";
    }
}

void dump_all_task(void)
{
    struct task_struct_t *task_temp;
    size_t max_used;
    sch_lock();
    pr_info("----------------------------------------------------------------------------------------\r\n");
    pr_info("|      task      |prio|    pid    |stack size|stack lave|stack max used| flag | status |\r\n");
    //pr_info("----------------------------------------------------------------------------------------\r\n");
    list_for_each_entry(task_temp, &task_list, tlist) {
        max_used = task_get_stack_max_used(task_temp);
        pr_info("|%16s|%4d|%11d|%10d|%10d|%14d|%6d|%8s|%s\r\n", 
            task_temp->name, task_temp->current_priority, task_temp->pid, task_temp->stack_size, 
            task_temp->stack_size - ((addr_t)task_temp->stack_addr - (addr_t)task_temp->sp),
            max_used, task_temp->flag, get_task_status_string(task_temp),
            max_used ? "" : "(task stack overflow)");
    }
    pr_info("----------------------------------------------------------------------------------------\r\n");
    sch_unlock();
}
