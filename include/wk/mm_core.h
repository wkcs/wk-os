/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __K_MM_CORE_H__
#define __K_MM_CORE_H__

#include <wk/kernel.h>

struct mm_pool_data_t {
    addr_t mm_pool_start;
    addr_t mm_pool_end;
    size_t block_num;
    size_t size;
    struct list_head *head;
};

extern struct mm_pool_data_t mm_pool_data;

size_t check_mm_pool(void);

#endif