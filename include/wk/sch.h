/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __SCH_H__
#define __SCH_H__

#include <wk/kernel.h>
#include <wk/task.h>

void sch_init(void);
void add_task_to_ready_list(struct task_struct_t *task);
void del_task_to_ready_list(struct task_struct_t *task);
struct task_struct_t *get_current_task(void);
void switch_task (void);

#endif