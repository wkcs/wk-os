/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include "arch_irq.h"

int irq_config(struct irq_config_t *config)
{

    if (config == NULL)
        return 0;
        
    NVIC_Init(&config->init_type);

    return 0;
}