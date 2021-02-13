/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <init/init.h>
#include <wk/kernel.h>
#include <wk/log.h>

extern addr_t __init_task_data_start;
extern addr_t __init_task_data_end;

//执行调度器启动前的任务初始化
void task_init_call(void)
{
    int ret;
    uint32_t i;
    uint32_t int_task_num = ((addr_t)&__init_task_data_end - (addr_t)&__init_task_data_start) /
                            sizeof(init_task_t);
    init_task_t *init_task_info;

    for(i = 0; i < int_task_num; i++) {
        init_task_info = (init_task_t *)((addr_t)&__init_task_data_start + sizeof(init_task_t) * i);

        if (init_task_info->task_init_func == NULL) {
            pr_err("%s: not find %s\r\n", __func__, init_task_info->name);
            continue;
        }

        pr_info("%s: start run %s\r\n", __func__, init_task_info->name);
        ret = init_task_info->task_init_func();
        if (ret != 0) {
            pr_err("%s: %s error\r\n", __func__, init_task_info->name);
            continue;
        }
    }
}

