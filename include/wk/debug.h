/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <wk/log.h>
#include <wk/printk.h>

inline void wk_debug(uint8_t log_type, const char *ex_string, const char *func, size_t line)
{
    pr_log(log_type, "(BUG)%s[%d]:%s\r\n", func, line, ex_string);
}

#endif