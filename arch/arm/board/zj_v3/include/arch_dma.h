/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __ARCH_DMA_H__
#define __ARCH_DMA_H__

#include "stm32f10x.h"

struct dma_config_t {
    struct clk_config_t *clk_config;
    struct irq_config_t *irq_config;
    uint32_t irq_type;
    DMA_Channel_TypeDef *ch;
    DMA_InitTypeDef init_type;
};

int dma_config(struct dma_config_t *config);
void dma_start(DMA_Channel_TypeDef *ch, uint16_t len);

#endif