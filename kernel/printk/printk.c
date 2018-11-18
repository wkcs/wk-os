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
	struct log_head_t head;
	int len;

	va_start(args, fmt);
	len = vsprintf(buf, fmt, args);
	va_end(args);

	head.grade = LOG_INFO;
	head.time = 124;
	head.log_size = len;

	len = write_log(&head, buf);

	return len;
}
