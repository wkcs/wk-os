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
#include <wk/cpu.h>
#include <wk/device.h>
#include <lib/stdio.h>
#include <lib/string.h>

#define REBOOT_FLASH_BASE 0x7f00000

static struct device *flash_dev = NULL;

static const char ls_help[] = "Usage:\r\nreboot\r\n    reboot now\r\nreboot boootloader\r\n    reboot to bootloader\r\n";

int reboot_main(const uint8_t argc, char *argv[])
{
    uint8_t reboot_flag;

    if (argc > 1) {
        printf("Parameter error\r\n");
        printf("%s\r\n", ls_help);
        return -EINVAL;
    }

    if (flash_dev == NULL) {
        flash_dev = device_find_by_name("spi-flash");
        if (flash_dev == NULL) {
            pr_err("%s: not find spi-flash device\r\n", __func__);
            return -ENODEV;
        }
    }

    if (argc == 1) {
        if (!strcmp(argv[0], "-h") || !strcmp(argv[0], "--help")) {
            printf("%s\r\n", ls_help);
            return 0;
        } if (!strcmp(argv[0], "bootloader")) {
            reboot_flag = 5;
            flash_dev->ops.write(flash_dev, REBOOT_FLASH_BASE, (void *)&reboot_flag, 1);
        } else {
            printf("Parameter error\r\n");
            printf("%s\r\n", ls_help);
            return -EINVAL;
        }
    }

    cpu_reboot();

    return 0;
}

cmd_init("reboot", reboot_main);
