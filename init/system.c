/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/task.h>

static struct task_struct_t *idle_task, *init_task;

static void idle_task_handle(void *argc)
{
    while(1)
    {
        
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