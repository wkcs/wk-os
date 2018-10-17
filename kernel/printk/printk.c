/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <lib/vsprintf.h>
#include <wk/log.h>
#include <lib/string.h>

int printk(const char *fmt, ...)
{
	va_list args;
	static char buf[100];
	struct log_type_t log;
	int r;

	va_start(args, fmt);
	r = vsprintf(buf, fmt, args);
	va_end(args);

	log.grade = LOG_NORMAL;
	log.buf = buf;
	log.log_size = r;

	write_log(&log);
	//usart_send(buf, strlen(buf));

	return r;
}