/*
 * Copyright (C) 2019 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <cmd/cmd.h>
#include <wk/log.h>
#include <wk/err.h>
#include <wk/task.h>
#include <wk/sch.h>
#include <lib/stdio.h>
#include <lib/string.h>

static const char task_show_help[] = "Usage: task_show\r\n    show all task";

static void show_all_task(void)
{
    struct task_struct_t *task_temp;
    struct list_head *task_list = get_task_list();
    size_t max_used;
    sch_lock();
    printf("----------------------------------------------------------------------------------------\r\n");
    printf("|      task      |prio|    pid    |stack size|stack lave|stack max used| flag | status |\r\n");
    list_for_each_entry(task_temp, task_list, tlist) {
        max_used = task_get_stack_max_used(task_temp);
        printf("|%16s|%4d|%11d|%10d|%10d|%14d|%6d|%8s|%s\r\n", 
            task_temp->name, task_temp->current_priority, task_temp->pid, task_temp->stack_size, 
            task_temp->stack_size - ((addr_t)task_temp->stack_addr - (addr_t)task_temp->sp),
            max_used, task_temp->flag, get_task_status_string(task_temp),
            max_used ? "" : "(task stack overflow)");
    }
    printf("----------------------------------------------------------------------------------------\r\n");
    sch_unlock();
}

int task_show_main(const uint8_t argc, char *argv[])
{
    if (argc > 1) {
        printf("Parameter error\r\n");
        printf("%s\r\n", task_show_help);
        return -EINVAL;
    }

    if (argc == 1) {
        if (!strcmp(argv[0], "-h") || !strcmp(argv[0], "--help")) {
            printf("%s\r\n", task_show_help);
            return 0;
        } else {
            printf("Parameter error\r\n");
            printf("%s\r\n", task_show_help);
            return -EINVAL;
        }
    }

    show_all_task();

    return 0;
}

cmd_init("task_show", task_show_main);

