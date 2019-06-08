/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/mm.h>
#include <wk/list.h>
#include <wk/config.h>

#include <lib/vsprintf.h>


void __mm_init(addr_t start, addr_t end, struct list_head *head)
{
    addr_t addr;
    struct mm_list_t *list;

    if ((end - start) % MM_BLOCK_SIZE != 0)
        return;
    
    for (addr = start; addr < end; addr += MM_BLOCK_SIZE)
    {
        list = (struct mm_list_t *)addr;
        list->pid = 0;
        list->size = 0;
        list->block_num = 0;
        list->flag = 0;
        INIT_LIST_HEAD(&list->list);
        list_add_tail(&list->list, head);
    }
}

int mm_pool_init(void)
{
    if (check_mm_pool() == 0)
        return -1;   
    INIT_LIST_HEAD(&mm_pool_data.head);
    __mm_init(mm_pool_data.mm_pool_start, mm_pool_data.mm_pool_end, &mm_pool_data.head);
    if (list_empty(&mm_pool_data.head))
        return -1;

    return 0;
}