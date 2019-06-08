/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __PRINTK_H__
#define __PRINTK_H__

#include <asm/types.h>
#include <wk/compiler.h>
#include <kernel_cfg.h>

#ifdef CONFIG_DEBUG
#define PR_LOG_BUG_SIZE 256
__printf(2,3) int pr_log(uint8_t grade, const char *fmt, ...);
void pr_log_enable(bool enable);
#else
inline void pr_log(__maybe_unused uint8_t grade, __maybe_unused const char *fmt, ...) {}
#endif

#endif