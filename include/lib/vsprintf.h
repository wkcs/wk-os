/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __VSPRINTF_H__
#define __VSPRINTF_H__

#include <wk/kernel.h>
#include <lib/stdarg.h>

__printf(3, 0) uint32_t vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
__printf(3, 4) uint32_t snprintf(char *buf, size_t size, const char *fmt, ...);
__printf(2, 0) uint32_t vsprintf(char *buf, const char *format, va_list arg_ptr);
__printf(2, 3) uint32_t sprintf(char *buf, const char *format, ...);

#endif