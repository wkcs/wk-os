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

#include "keyboard.h"

static const char led_help[] = "Usage:\r\nled on\r\n    backlight on\r\nled off\r\n    backlight off\r\n";

int led_main(const uint8_t argc, char *argv[])
{
    if (argc == 1) {
        if (!strcmp(argv[0], "-h") || !strcmp(argv[0], "--help")) {
            printf("%s\r\n", led_help);
        } else if (!strcmp(argv[0], "on")) {
            led_backlight_on_off(BACKLIGHT_ON);
        } else if (!strcmp(argv[0], "off")) {
            led_backlight_on_off(BACKLIGHT_OFF);
        } else {
            printf("Parameter error\r\n");
            printf("%s\r\n", led_help);
            return -EINVAL;
        }
    } else {
        printf("Parameter error\r\n");
        printf("%s\r\n", led_help);
        return -EINVAL;
    }

    return 0;
}

cmd_init("led", led_main);