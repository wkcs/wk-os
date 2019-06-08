/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include "asm_irq.h"

__addr_t interrupt_from_task, interrupt_to_task;
__wk_u32_t switch_interrupt_flag;

void asm_irq_init(void)
{
    interrupt_from_task = 0;
    interrupt_to_task = 0;
    switch_interrupt_flag = 0;
}