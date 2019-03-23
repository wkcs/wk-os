/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/task.h>
#include <wk/sch.h>
#include <wk/mm.h>
#include <wk/sem.h>
#include <wk/err.h>
#include <wk/console.h>
#include <cmd/cmd.h>
#include <init/init.h>
#include <lib/stdio.h>
#include <lib/string.h>

static struct task_struct_t *console_task;
static sem_t *console_sem;

static char cmd_buf[257];
static size_t cmd_len;
static char *cmd;
static uint8_t argc;
static char *argv[20];

void cmd_buf_deal(char *buf, size_t len)
{
    cmd_len = len;
    memcpy(cmd_buf, buf, len);
    sem_send_one(console_sem);
}

void cmd_analysis(void)
{
    size_t i;
    bool start = false;

    argc = 0;
    cmd = NULL;
    for (i = 0; i < cmd_len; i++) {
        if (cmd_buf[i] == ' ') {
            if (start) {
                cmd_buf[i] = 0;
                if (((i + 1) < cmd_len) && (cmd_buf[i + 1] != ' ')) {
                    if (argc >= 20) {
                        pr_err("cmd(%s) more than 20 parameters\r\n", cmd);
                        return;
                    }
                    i++;
                    argv[argc] = cmd_buf + i;
                    argc++;
                }
            }
        } else {
            if (!start) {
                cmd = cmd_buf + i;
                start = true;
            }
        }
    }
    cmd_buf[i] = 0;
}

static void console_task_handle(__maybe_unused void *para)
{
    console_sem = sem_alloc(0, console_task->pid);
    if (console_sem == NULL) {
        pr_err("%s: alloc sem error\r\n", __func__);
        return;
    }
    while(1)
    {
        printf("wk-os-> ");
        sem_get(console_sem);
        cmd_analysis();
        if (cmd != NULL) {
            cmd_run(cmd, argc, argv);
        }
    }
}

int console_task_init(void)
{
    console_task = task_create("console", console_task_handle, NULL, 512, 1, 10, NULL, NULL);

    if (console_task)
        task_ready(console_task);
    else {
        pr_fatal("%s[%d]:create console task error\r\n", __func__, __LINE__);
        return -ENODEV;
    }

    return 0;
}
task_init(console_task_init);