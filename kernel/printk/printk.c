/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/printk.h>
#include <lib/vsprintf.h>
#include <lib/string.h>
#include <wk/log.h>

int pr_log(uint8_t grade, const char *fmt, ...)
{
	va_list args;
	static char buf[PR_LOG_BUG_SIZE];
	struct log_head_t head;
	int len;

	va_start(args, fmt);
	len = vsprintf(buf, fmt, args);
	va_end(args);

	head.grade = grade;
	head.time = 0;
	head.log_size = len;

	len = write_log(&head, buf);

	return len;
}
