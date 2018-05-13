/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __WK_MM_H__
#define __WK_MM_H__

#include <wk/kernel.h>
#include <wk/list.h>
#include <wk/cpu.h>
#include <wk/pid.h>
#include <wk/config.h>

typedef wk_u8_t mm_flag_t;

struct mm_list_t {
    wk_pid_t pid;
    size_t size;
    size_t block_num;
    mm_flag_t flag;
//#define MM_FLAG_1 BIT(1)
    struct list_head list;
};

struct list_head *__mm_init(addr_t start, addr_t end);
int mm_pool_init(void);
void *__mm_alloc(size_t size, mm_flag_t flag, wk_pid_t pid);
int __mm_free(void *addr);

#endif