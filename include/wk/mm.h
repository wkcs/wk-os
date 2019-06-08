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

struct mm_pool_data_t {
    addr_t mm_pool_start;
    addr_t mm_pool_end;
    size_t block_num;
    size_t size;
    struct list_head head;
};

extern struct mm_pool_data_t mm_pool_data;

typedef wk_u8_t mm_flag_t;

struct mm_list_t {
    wk_pid_t pid;
    size_t size;
    size_t block_num;
    mm_flag_t flag;
//#define MM_FLAG_1 BIT(1)
    struct list_head list;
};

size_t check_mm_pool(void);
void __mm_init(addr_t start, addr_t end, struct list_head *head);
int mm_pool_init(void);
void *wk_alloc(size_t size, mm_flag_t flag, wk_pid_t pid);
int wk_free(void *addr);
void wk_free_by_pid(wk_pid_t pid);
void *stack_alloc(size_t stack_size);
int stack_free(size_t stack_size, void *addr);

#endif