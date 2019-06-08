/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __WK_IRQ_H__
#define __WK_IRQ_H__

#include <wk/kernel.h>

void wk_interrupt_enter(void);
void wk_interrupt_leave(void);
uint8_t get_irq_level(void);

#endif