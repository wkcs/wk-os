/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/sch.h>
#include <wk/list.h>
#include <wk/cpu.h>
#include <lib/string.h>

struct list_head ready_task_list[MAX_PRIORITY];
static struct task_struct_t *current_task;
uint8_t current_priority;
static uint32_t scheduler_lock_nest;

extern volatile uint8_t interrupt_nest;

#if MAX_PRIORITY > 32
uint32_t ready_task_priority_group;
uint8_t ready_task_table[32];
#else
uint32_t ready_task_priority_group;
#endif

void sch_init(void)
{
    register addr_t offset;

    scheduler_lock_nest = 0;

    for (offset = 0; offset < MAX_PRIORITY; offset ++)
        INIT_LIST_HEAD(&ready_task_list[offset]);

    current_priority = MAX_PRIORITY - 1;
    current_task = NULL;

    ready_task_priority_group = 0;

#if MAX_PRIORITY > 32
    /* initialize ready table */
    memset(ready_task_table, 0, sizeof(ready_task_table));
#endif

}

void sch_start(void)
{
    register struct task_struct_t *to_task;
    register addr_t highest_ready_priority;

#if MAX_PRIORITY > 32
    register addr_t offset;

    offset = __rt_ffs(ready_task_priority_group) - 1;
    highest_ready_priority = (offset << 3) + __rt_ffs(ready_task_table[offset]) - 1;
#else
    highest_ready_priority = __rt_ffs(ready_task_priority_group) - 1;
#endif

    /* get switch to task */
    to_task = list_entry(ready_task_list[highest_ready_priority].next,
                              struct task_struct_t,
                              list);
    current_task = to_task;
    to_task->status = TASK_RUNING;
    
    /* switch to new task */
    context_switch_to((addr_t)&to_task->sp);

    /* never come back */
}

void switch_task (void)
{
    addr_t level;
    struct task_struct_t *to_task;
    struct task_struct_t *from_task;

    /* disable irq */
    level = disable_irq_save();

    /* check the scheduler is enabled or not */
    if (scheduler_lock_nest == 0)
    {
        register addr_t highest_ready_priority;

#if MAX_PRIORITY <= 32
        highest_ready_priority = __rt_ffs(ready_task_priority_group) - 1;
#else
        register addr_t offset;

        offset = __rt_ffs(ready_task_priority_group) - 1;
        highest_ready_priority = (offset << 3) + __rt_ffs(ready_task_table[offset]) - 1;
#endif

        /* get switch to task */
        to_task = list_entry(ready_task_list[highest_ready_priority].next,
                                  struct task_struct_t,
                                  list);

        to_task->status = TASK_RUNING;
        /* if the destination task is not the same as current task */
        if (to_task != current_task) {
            current_priority = (uint8_t)highest_ready_priority;
            from_task = current_task;
            current_task = to_task;

            if (interrupt_nest == 0) {
                context_switch((addr_t)&from_task->sp, (addr_t)&to_task->sp);

                /* enable interrupt */
                enable_irq_save(level);

            } else {
                context_switch_interrupt((addr_t)&from_task->sp, (addr_t)&to_task->sp);
                /* enable interrupt */
                enable_irq_save(level);
            }
        } else {
            /* enable interrupt */
            enable_irq_save(level);
        }
    } else {
        /* enable interrupt */
        enable_irq_save(level);
    }
}


void add_task_to_ready_list(struct task_struct_t *task)
{
    register addr_t level;

    /* disable interrupt */
    level = disable_irq_save();

    /* change status */
    task->status = TASK_READY;

    /* insert task to ready list */
    list_add_tail(&(task->list), &(ready_task_list[task->current_priority]));

#if MAX_PRIORITY > 32
    ready_task_table[task->offset] |= task->prio_mask;
#endif
    ready_task_priority_group |= task->offset_mask;

    /* enable interrupt */
    enable_irq_save(level);
}

void del_task_to_ready_list(struct task_struct_t *task)
{
    register addr_t level;

    /* disable interrupt */
    level = disable_irq_save();

    /* remove task from ready list */
    list_del(&(task->list));
    if (list_empty(&(ready_task_list[task->current_priority])))
    {
#if MAX_PRIORITY > 32
        ready_task_table[task->offset] &= ~task->prio_mask;
        if (ready_task_table[task->offset] == 0)
        {
            ready_task_priority_group &= ~task->offset_mask;
        }
#else
        ready_task_priority_group &= ~task->prio_mask;
#endif
    }

    /* enable interrupt */
    enable_irq_save(level);
}

inline struct task_struct_t *get_current_task(void)
{
    return current_task;
}