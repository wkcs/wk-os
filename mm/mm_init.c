/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/mm_core.h>
#include <wk/mm.h>
#include <wk/list.h>
#include <wk/config.h>

#include <lib/vsprintf.h>


struct list_head *__mm_init(addr_t start, addr_t end)
{
    addr_t addr;
    struct mm_list_t *list;
    struct list_head *head;

    if ((end - start) % MM_BLOCK_SIZE != 0)
        return NULL;

    list = (struct mm_list_t *)(start); 
    list->pid = 0;
    list->size = 0;
    list->block_num = 0;
    list->flag = 0;
    INIT_LIST_HEAD(&list->list);
    head = &list->list;
    
    for (addr = start + MM_BLOCK_SIZE; addr < end; addr += MM_BLOCK_SIZE)
    {
        list = (struct mm_list_t *)addr;
        list->pid = 0;
        list->size = 0;
        list->block_num = 0;
        list->flag = 0;
        INIT_LIST_HEAD(&list->list);
        list_add_tail(&list->list, head);
    }

    return head;
}

int mm_pool_init(void)
{
    if (check_mm_pool() == 0)
        return -1;
    printf("mm_pool_start = %p, mm_pool_end = %p\r\n", mm_pool_data.mm_pool_start, mm_pool_data.mm_pool_end);    
    mm_pool_data.head = __mm_init(mm_pool_data.mm_pool_start, mm_pool_data.mm_pool_end);
    if (mm_pool_data.head == NULL)
        return -1;

    return 0;
}