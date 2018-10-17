/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/log.h>
#include <lib/string.h>
#include <lib/vsprintf.h>

#include "arch_uart.h"

#ifdef KERNEL_LOG_ENABLE

extern addr_t __log_buf_start, __log_buf_end;
static addr_t log_buf_start, log_buf_end, log_buf_write, log_buf_read, log_start;


struct list_head *kernel_log_list;

int __init log_server_init(void)
{
    log_buf_start = (addr_t)&__log_buf_start;
    log_buf_end = (addr_t)&__log_buf_end;
    log_buf_write = log_buf_read = log_start = log_buf_start;

    printf("log_buf_start = %p, log_buf_end = %p\r\n", log_buf_start, log_buf_end);
    return 0;
}

static inline void copy_to_log_buf(char *buf, uint32_t len)
{
    if (len > KERNEL_LOG_BUF_SIZE){
        if (log_buf_write == log_buf_end) {
            *(char *)log_buf_start = '\n';
            if (log_start > log_buf_write && log_start < log_buf_write + 1)
                log_start = log_buf_write + 1;
            if (log_buf_read > log_buf_write && log_buf_read < log_buf_write + 1)
                log_buf_read = log_buf_write + 1;
            log_buf_write += 1;
        }
    }

    if (len <= log_buf_end - log_buf_write) {
        memcpy((char *)log_buf_write, buf, len);
        //printf("log_buf_write = %p, line = %d\r\n", (char *)log_buf_write, __LINE__);
        if (log_start > log_buf_write && log_start < log_buf_write + len)
            log_start = log_buf_write + len;
        if (log_buf_read > log_buf_write && log_buf_read < log_buf_write + len)
            log_buf_read = log_buf_write + len;
        log_buf_write += len;
    } else if (log_buf_write == log_buf_end) {
        memcpy((char *)log_buf_start, buf, len);
        //printf("log_buf_write = %p, line = %d\r\n", (char *)log_buf_write, __LINE__);
        log_buf_write = log_buf_start + len;
        log_start = log_buf_write;
        if (log_buf_read < log_buf_write)
            log_buf_read = log_buf_write;
    } else {
        memcpy((char *)log_buf_write, buf, log_buf_end - log_buf_write);
        //printf("log_buf_write = %p, line = %d\r\n", (char *)log_buf_write, __LINE__);
        memcpy((char *)log_buf_start, buf + log_buf_end - log_buf_write, len + log_buf_write - log_buf_end);
        //printf("log_buf_write = %p, line = %d\r\n", (char *)log_buf_write, __LINE__);
        if (log_buf_read > log_buf_write)
            log_buf_read = log_buf_start + len + log_buf_write - log_buf_end;
        log_buf_write = log_buf_start + len + log_buf_write - log_buf_end;
        log_start = log_buf_write;
    }
}

static inline void write_time_to_log_buf(uint64_t time)
{
    char buf[13];

    sprintf((char *)buf, "%06d.%06d", (uint32_t)(time / 1000000), (uint32_t)(time % 1000000));
    copy_to_log_buf(buf, 13);
}

static inline void write_grade_to_log_buf(uint8_t grade)
{
    char buf[10];

    switch(grade) {
        case LOG_FATAL: 
            sprintf((char *)buf, "[FATAL]"); 
            copy_to_log_buf(buf, 7);
            break;
        case LOG_URGENT:
            sprintf((char *)buf, "[URGENT]"); 
            copy_to_log_buf(buf, 8);
            break;
        case LOG_CRITICAL:
            sprintf((char *)buf, "[CRITICAL]"); 
            copy_to_log_buf(buf, 10); 
            break;
        case LOG_ERROR:
            sprintf((char *)buf, "[ERROR]"); 
            copy_to_log_buf(buf, 7); 
            break;
        case LOG_WARNING:
            sprintf((char *)buf, "[WARNING]"); 
            copy_to_log_buf(buf, 9); 
            break;
        case LOG_INFO:
            sprintf((char *)buf, "[INFO]"); 
            copy_to_log_buf(buf, 6); 
            break;
        case LOG_NORMAL:
            sprintf((char *)buf, "[NORMAL]"); 
            copy_to_log_buf(buf, 8); 
            break;
        default:
            break;
    }
}

void __write_log(struct log_type_t *log)
{
    write_time_to_log_buf(get_kernel_run_time());
    write_grade_to_log_buf(log->grade);
    copy_to_log_buf(log->buf, log->log_size);
}

void write_log(struct log_type_t *log)
{
    __write_log(log);
}

uint32_t __read_log(void)
{
    uint32_t len = 0;

    if (log_buf_write > log_buf_read) {
        len = log_buf_write - log_buf_read;
        usart_send((char *)log_buf_read, len);
        log_buf_read = log_buf_write;
    } else if (log_buf_write < log_buf_read){
        len = log_buf_end - log_buf_read;
        usart_send((char *)log_buf_read, len);
        usart_send((char *)log_buf_start, log_buf_write - log_buf_start);
        len += log_buf_write - log_buf_start;
    }
    
    return len;
}

uint32_t read_log(void)
{
    uint32_t len = 0;

    len = __read_log();

    return len;
}

#endif
