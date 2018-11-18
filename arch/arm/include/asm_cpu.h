/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __ASM_CPU_H__
#define __ASM_CPU_H__

#include <asm_types.h>

#define USE_CPU_FFS

inline int __rt_ffs(int value)
{
    return __builtin_ffs(value);
}

extern __addr_t *stack_init(void *task_entry, void *parameter, __addr_t *stack_addr, void *task_exit);
extern __addr_t disable_irq_save();
extern void enable_irq_save(__addr_t level);
extern void context_switch_interrupt(__addr_t from, __addr_t to);
extern void context_switch(__addr_t from, __addr_t to);
extern void context_switch_to(__addr_t to);
extern void asm_irq_init(void);

#endif