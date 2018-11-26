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

__init int log_server_init(void)
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

size_t write_log(char * buf, size_t len)
{
    register addr_t level;
    register size_t size = 0;

    if (!logs.status)
        return 0;

    level = disable_irq_save();

    size = write_fifo_buf_size(&logs.log_buf, (uint8_t *)buf, len);

    enable_irq_save(level);

    send_to_console();

    return size;
}

size_t read_log(char *buf, size_t free_size)
{
    register addr_t level;
    size_t len = 0;

    if (!logs.status)
        return 0;

    level = disable_irq_save();

    if (logs.log_buf.buf_size - logs.log_buf.free_size >= free_size)
        len = read_fifo_buf_size(&logs.log_buf, (uint8_t *)buf, free_size);
    else
        len = read_fifo_buf_size(&logs.log_buf, (uint8_t *)buf, logs.log_buf.buf_size - logs.log_buf.free_size);

    enable_irq_save(level);
    return len;
}

#endif
