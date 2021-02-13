/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __ARCH_IRQ_H__
#define __ARCH_IRQ_H__

#include "stm32f4xx.h"

struct irq_config_t {
    NVIC_InitTypeDef init_type;
};

int irq_config(struct irq_config_t *config);

#endif