/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __WK_CMD_H__
#define __WK_CMD_H__

#include <wk/kernel.h>
#include <wk/section.h>

typedef struct {
    char *name;
    int (*cmd_func)(const uint8_t argc, char *argv[]);
} cmd_list_t;

#define cmd_init(func_name, func) \
    __visible __cmd const cmd_list_t __maybe_unused __cmd_##func = { \
        .name = func_name, \
        .cmd_func = func \
    }

int cmd_run(const char *name, const uint8_t argc, char *argv[]);
void print_all_cmd(void);

#endif