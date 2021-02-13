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
#include <wk/mm.h>
#include <wk/irq.h>
#include <wk/console.h>
#include <lib/stdio.h>
#include <lib/string.h>

static const char mm_help[] = "Usage: mm\r\n    -i/--info  show memory info\r\n    -d/--dump  dump memory list\r\n";

void mm_dump_list(void)
{
    struct mm_list_t *list_temp;
    uint32_t i = 0;

    list_for_each_entry(list_temp, &mm_pool_data.head, list) {
        i++;
        printf("block[%d]----addr = %p\r\n", i, list_temp);
        printf("         |---size = %d\r\n", list_temp->size);
        printf("         -----num = %d\r\n", list_temp->block_num);
    }
}

void mm_show_info(void)
{
    struct mm_list_t *list_temp;
    uint32_t i = 0;
    uint32_t block_num = 0;
    bool free_mem_find = false;

    printf("  num     size     free  block_num\r\n");
    list_for_each_entry(list_temp, &mm_pool_data.head, list) {
        if (list_temp->block_num == 0) {
            if (!free_mem_find)
                free_mem_find = true;
            block_num++;
        } else {
            if (free_mem_find) {
                printf("%5d %8d %8d %10d\r\n", i, block_num * MM_BLOCK_SIZE,
                    block_num * MM_BLOCK_SIZE - sizeof(*list_temp), block_num);
                block_num = 0;
                free_mem_find = false;
            }
            printf("%5d %8d %8d %10d\r\n", i, list_temp->block_num * MM_BLOCK_SIZE,
                0, list_temp->block_num);
            i++;
        }
    }
    if (block_num !=0)
        printf("%5d %8d %8d %10d\r\n", i, block_num * MM_BLOCK_SIZE,
            block_num * MM_BLOCK_SIZE - sizeof(*list_temp), block_num);
}

int mm_main(const uint8_t argc, char *argv[])
{
    if (argc == 1) {
        if (!strcmp(argv[0], "-h") || !strcmp(argv[0], "--help")) {
            printf("%s\r\n", mm_help);
        } else if (!strcmp(argv[0], "-i") || !strcmp(argv[0], "--info")) {
            mm_show_info();
        } else if (!strcmp(argv[0], "-d") || !strcmp(argv[0], "--dump")) {
            mm_dump_list();
        } else {
            printf("Parameter error\r\n");
            printf("%s\r\n", mm_help);
            return -EINVAL;
        }
    } else {
        printf("Parameter error\r\n");
        printf("%s\r\n", mm_help);
        return -EINVAL;
    }

    return 0;
}

cmd_init("mm", mm_main);