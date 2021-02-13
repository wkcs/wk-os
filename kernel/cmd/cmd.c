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
#include <lib/stdio.h>
#include <lib/string.h>

extern addr_t __cmd_list_data_start;
extern addr_t __cmd_list_data_end;

//执行命令
int cmd_run(const char *name, const uint8_t argc, char *argv[])
{
    uint32_t i;
    uint32_t cmd_num = ((addr_t)&__cmd_list_data_end - (addr_t)&__cmd_list_data_start) /
                            sizeof(cmd_list_t);
    cmd_list_t *cmd_list_info;

    for(i = 0; i < cmd_num; i++) {
        cmd_list_info = (cmd_list_t *)((addr_t)&__cmd_list_data_start + sizeof(cmd_list_t) * i);
        if (!strcmp(cmd_list_info->name, name)) {
            if (cmd_list_info->cmd_func) {
                return cmd_list_info->cmd_func(argc, argv);
            } else {
                pr_err("cmd(%s) has no entry function\r\n", name);
                printf("%s: No entry function\r\n", name);
                return -ENFILE;
            }
        }
    }
    printf("%s: Command not found\r\n", name);
    return -ENFILE;
}

void print_all_cmd(void)
{
    uint32_t i;
    uint32_t cmd_num = ((addr_t)&__cmd_list_data_end - (addr_t)&__cmd_list_data_start) /
                            sizeof(cmd_list_t);
    cmd_list_t *cmd_list_info;

    for(i = 0; i < cmd_num; i++) {
        cmd_list_info = (cmd_list_t *)((addr_t)&__cmd_list_data_start + sizeof(cmd_list_t) * i);
        printf("%s\r\n", cmd_list_info->name);
    }
}

