/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/task.h>
#include <wk/mm.h>

static struct task_struct_t *idle_task, *init_task;
extern struct list_head close_task_list;

static void idle_task_handle(void *argc)
{
    register addr_t level;

    struct task_struct_t *task_temp;
    while(1)
    {
        level = disable_irq_save();
        if (!list_empty(&close_task_list)) {
            task_temp = list_entry(close_task_list.next, struct task_struct_t, list);
            list_del(&task_temp->list);
        } else 
            task_temp = NULL;
        enable_irq_save(level);

        if (task_temp != NULL) {
            wk_free_by_pid(task_temp->pid);
            stack_free(task_temp->stack_size, task_temp->stack_addr);
            wk_free(task_temp);
        }
    }
}

static void init_task_handle(void *argc)
{
    
}

void system_init(void)
{
    init_task = task_create("init", init_task_handle, NULL, 256, 0, 1000, NULL, NULL);
    
    if (init_task)
        task_ready(init_task);
    else 
        pr_fatal("%s[%d]:create init task error\r\n", __func__, __LINE__);

    idle_task = task_create("idle", idle_task_handle, NULL, 256, MAX_PRIORITY - 1, 1000, NULL, NULL);

    if (idle_task)
        task_ready(idle_task);
    else 
        pr_fatal("%s[%d]:create idle task error\r\n", __func__, __LINE__);
}