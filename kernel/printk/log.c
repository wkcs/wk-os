/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/log.h>
#include <wk/mm.h>
#include <lib/string.h>
#include <lib/vsprintf.h>
#include <lib/fifo_buf.h>

#include "board.h"

#ifdef KERNEL_LOG_ENABLE

struct log_server {
    struct fifo_buf_t log_buf;
    addr_t log_start;
    bool status;
};

static struct log_server logs = {
    .status = false,
};

int __init log_server_init(void)
{
    logs.log_buf.buf_start = (addr_t)wk_alloc(KERNEL_LOG_BUF_SIZE, 0, 0);
    //logs.log_buf.buf_start = log_buf_buf;
    logs.log_buf.buf_end = logs.log_buf.buf_start + KERNEL_LOG_BUF_SIZE;
    logs.log_buf.read = logs.log_buf.buf_start;
    logs.log_buf.write = logs.log_buf.buf_start;
    logs.log_buf.free_size = KERNEL_LOG_BUF_SIZE;
    logs.log_buf.buf_size = KERNEL_LOG_BUF_SIZE;
    logs.log_start = logs.log_buf.buf_start;
    logs.status = true;

    return 0;
}

size_t write_log(struct log_head_t *head, char * buf)
{
    register addr_t level;
    register size_t size = 0;

    if (!logs.status)
        return 0;

    level = disable_irq_save();

    size += write_fifo_buf_size(&logs.log_buf, (uint8_t *)head, sizeof(struct log_head_t));
    size += write_fifo_buf_size(&logs.log_buf, (uint8_t *)buf, head->log_size);

    send_to_console();

    enable_irq_save(level);

    return size;
}

size_t get_log_len(struct log_head_t *head)
{
    if (!logs.status)
        return 0;

    switch(head->grade) {
        case LOG_FATAL:
        case LOG_ERROR:
            return 20 + head->log_size;
        case LOG_WARNING:
            return 22 + head->log_size;
        case LOG_INFO:
            return 19 + head->log_size;
        default:
            return 0;
    }
}

size_t read_next_log_len(void)
{
    register size_t len;
    struct log_head_t head;

    if (!logs.status)
        return 0;

    if (!read_fifo_buf_size_test(&logs.log_buf, (uint8_t *)&head, sizeof(struct log_head_t))) {
        return 0;
    }

    len = get_log_len(&head);

    return len; 
}

size_t __read_log_one(char *buf)
{
    register size_t len;
    struct log_head_t head;

    if (!logs.status)
        return 0;

    if (!read_fifo_buf_size(&logs.log_buf, (uint8_t *)&head, sizeof(struct log_head_t)))
        return 0;
    switch(head.grade) {
        case LOG_FATAL:
            sprintf(buf, "%06d.%06d[FATAL]",  (uint32_t)(head.time << 32), (uint32_t)(head.time));
            len = 20;
            buf += 20;
            break;
        case LOG_ERROR:
            sprintf(buf, "%06d.%06d[ERROR]",  (uint32_t)(head.time << 32), (uint32_t)(head.time));
            len = 20;
            buf += 20;
            break;
        case LOG_WARNING:
            sprintf(buf, "%06d.%06d[WARNING]",  (uint32_t)(head.time << 32), (uint32_t)(head.time));
            len = 22;
            buf += 22;
            break;
        case LOG_INFO:
            sprintf(buf, "%06d.%06d[INFO]",  (uint32_t)(head.time << 32), (uint32_t)(head.time));
            len = 19;
            buf += 19;
            break;
        default:
            return 0;
    }
    len += read_fifo_buf_size(&logs.log_buf, (uint8_t *)buf, head.log_size);

    return len;
}

size_t read_log(char *buf, size_t free_size)
{
    register addr_t level;
    size_t len = 0, len_old, size;

    if (!logs.status)
        return 0;

    level = disable_irq_save();

    size = read_next_log_len();
    if(!size) {
        enable_irq_save(level);
        return len;
    }
    while(free_size > size) {
        len_old = len;
        len += __read_log_one(buf);
        if(len == len_old) {
            enable_irq_save(level);
            return 0;
        }
        buf += size;
        free_size -= size;
        size = read_next_log_len();
        if(!size) {
            enable_irq_save(level);
            return len;
        }
    }

    enable_irq_save(level);
    return len;
}

#endif
