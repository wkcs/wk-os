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
#include <wk/device.h>
#include <lib/stdio.h>
#include <lib/string.h>

static const char ls_help[] = "Usage: ls\r\n    -c/--cmd  show all cmd\r\n    -d/--device  show all device\r\n";

int ls_main(const uint8_t argc, char *argv[])
{
    struct device *dev_tmp;

    if (argc > 1) {
        printf("Parameter error\r\n");
        printf("%s\r\n", ls_help);
        return -EINVAL;
    }

    if (argc == 1) {
        if (!strcmp(argv[0], "-h") || !strcmp(argv[0], "--help")) {
            printf("%s\r\n", ls_help);
            return 0;
        } if (!strcmp(argv[0], "-c") || !strcmp(argv[0], "--cmd")) {
            print_all_cmd();
            return 0;
        } if (!strcmp(argv[0], "-d") || !strcmp(argv[0], "--device")) {
            printf("  name    ref_count  \r\n");
            list_for_each_entry(dev_tmp, &sys_device_list, list) {
                printf("%8s%13d\r\n", dev_tmp->name, dev_tmp->ref_count);
            }
            return 0;
        } else {
            printf("Parameter error\r\n");
            printf("%s\r\n", ls_help);
            return -EINVAL;
        }
    }

    print_all_cmd();

    return 0;
}

cmd_init("ls", ls_main);

