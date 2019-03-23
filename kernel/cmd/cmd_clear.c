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
#include <wk/console.h>
#include <lib/stdio.h>
#include <lib/string.h>

static const char clear_help[] = "Usage: clear\r\n    Clear screen display";

int clear_main(const uint8_t argc, char *argv[])
{
    if (argc > 1) {
        printf("Parameter error\r\n");
        printf("%s\r\n", clear_help);
        return -EINVAL;
    }

    if (argc == 1) {
        if (!strcmp(argv[0], "-h") || !strcmp(argv[0], "--help")) {
            printf("%s\r\n", clear_help);
            return 0;
        } else {
            printf("Parameter error\r\n");
            printf("%s\r\n", clear_help);
            return -EINVAL;
        }
    }

    console_send_data("\x1b[2J\x1b[H", strlen("\x1b[2J\x1b[H"));

    return 0;
}

cmd_init("clear", clear_main);

