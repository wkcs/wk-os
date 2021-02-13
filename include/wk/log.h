/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __LOG_H__
#define __LOG_H__

#include <asm/types.h>
#include <wk/section.h>

#define KERNEL_LOG_ENABLE

#ifndef KERNEL_LOG_BUF_SIZE
    #define KERNEL_LOG_BUF_SIZE 1024
#endif

struct log_head_t {
    uint8_t grade;
#define LOG_FATAL 1U
#define LOG_ERROR 1U << 1
#define LOG_WARNING 1U << 2
#define LOG_INFO 1U << 3
    uint64_t time;
    size_t log_size;
};

int __init log_server_init(void);
size_t write_log(char * buf, size_t len);
size_t read_log(char *buf, size_t free_size);

#endif