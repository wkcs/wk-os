/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __LOG_H__
#define __LOG_H__

#include <wk/kernel.h>
#include <wk/list.h>

#define KERNEL_LOG_ENABLE

#ifndef KERNEL_LOG_BUF_SIZE
    #define KERNEL_LOG_BUF_SIZE 1024
#endif

struct log_type_t {
    uint8_t grade;
#define LOG_FATAL 1
#define LOG_URGENT 1 << 1
#define LOG_CRITICAL 1 << 2
#define LOG_ERROR 1 << 3
#define LOG_WARNING 1 << 4
#define LOG_INFO 1 << 5
#define LOG_NORMAL 1 << 6
#define LOG_NULL 1 << 7
    char *buf;
    size_t log_size;
};

int log_server_init(void);
void __write_log(struct log_type_t *log);
void write_log(struct log_type_t *log);
uint32_t __read_log(void);
uint32_t read_log(void);

#endif