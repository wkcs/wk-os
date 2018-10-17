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

uint32_t vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
uint32_t snprintf(char *buf, size_t size, const char *fmt, ...);
uint32_t vsprintf(char *buf, const char *format, va_list arg_ptr);
uint32_t sprintf(char *buf, const char *format, ...);

#endif