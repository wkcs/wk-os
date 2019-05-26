/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __ASM_STACKTRACE_H__
#define __ASM_STACKTRACE_H__

#include <wk/kernel.h>

struct stackframe {
	/*
	 * FP member should hold R7 when CONFIG_THUMB2_KERNEL is enabled
	 * and R11 otherwise.
	 */
	unsigned long fp;
	unsigned long sp;
	unsigned long lr;
	unsigned long pc;
};

#endif