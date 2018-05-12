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

#define mm_flag_t u8;

struct mm_list_t {
    pid_t pid;
    size_t size;s
    size_t block_num;
    mm_flag_t flag;
#define MM_FLAG_1 BIT(1)
    struct list_head *list;
    __attribute__ ((aligned (MM_ALIGN)))
};

#endif