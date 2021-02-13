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

#ifdef CONFIG_DEBUG

static const char log_help[] = "Usage:\r\nlog --enable/-e\r\n    Enable pr_log\r\nlog --disable/-d\r\n    Disable pr_log";

int log_main(const uint8_t argc, char *argv[])
{
    if (argc == 1) {
        if (!strcmp(argv[0], "-h") || !strcmp(argv[0], "--help")) {
            printf("%s\r\n", log_help);
        } else if (!strcmp(argv[0], "-e") || !strcmp(argv[0], "--enable")) {
            pr_log_enable(true);
        } else if (!strcmp(argv[0], "-d") || !strcmp(argv[0], "--disable")) {
            pr_log_enable(false);
        } else {
            printf("Parameter error\r\n");
            printf("%s\r\n", log_help);
            return -EINVAL;
        }
    } else {
        printf("Parameter error\r\n");
        printf("%s\r\n", log_help);
        return -EINVAL;
    }

    return 0;
}

cmd_init("log", log_main);

#endif