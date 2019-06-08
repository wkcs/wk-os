/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __WK_KERNEL_H__
#define __WK_KERNEL_H__

#include <asm/types.h>
#include <kernel_cfg.h>
#include <wk/compiler.h>
#include <wk/section.h>
#include <wk/printk.h>
#include <wk/log.h>
#include <wk/debug.h>

#define TIMER_TICK_MAX WK_U32_MAX

#define wk_offsetof(type, member) (size_t)&(((type *)0)->member)

/**
 * wk_container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define wk_container_of(ptr, type, member) ({				\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);   \
	(type *)( (size_t)__mptr - wk_offsetof(type, member) );})

#define get_kernel_run_time() 0

#define pr_fatal(fmt, ...)  \
	pr_log(LOG_FATAL, fmt, ## __VA_ARGS__)

#define pr_err(fmt, ...)  \
	pr_log(LOG_ERROR, fmt, ## __VA_ARGS__)

#define pr_warning(fmt, ...)  \
	pr_log(LOG_WARNING, fmt, ## __VA_ARGS__)

#define pr_info(fmt, ...)  \
	pr_log(LOG_INFO, fmt, ## __VA_ARGS__)

#define WK_FATAL(ex)  \
	if (!(ex)) {  \
		wk_debug(LOG_FATAL, #ex, __func__, __LINE__);  \
	}

#define WK_ERROR(ex)  \
	if (!(ex)) {  \
		wk_debug(LOG_ERROR, #ex, __func__, __LINE__);  \
	}

#define WK_WARNING(ex)  \
	if (!(ex)) {  \
		wk_debug(LOG_WARNING, #ex, __func__, __LINE__);  \
	}

extern addr_t kernel_running;

#endif