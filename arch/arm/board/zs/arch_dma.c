/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 *
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>

#include "arch_dma.h"
#include "arch_clk.h"
#include "arch_irq.h"

int dma_config(struct dma_config_t *config)
{

    if (config == NULL)
        return 0;
        
    clk_enable_all(config->clk_config);

    DMA_DeInit(config->ch);   
	DMA_Init(config->ch, &config->init_type);

    if (config->irq_config) {
        irq_config(config->irq_config);
        DMA_ITConfig(config->ch, config->irq_type, ENABLE);
    }
	
    return 0;
} 

void dma_start(DMA_Channel_TypeDef *ch, uint16_t len)
{
	DMA_Cmd(ch, DISABLE );
 	DMA_SetCurrDataCounter(ch, len);
 	DMA_Cmd(ch, ENABLE); 
}

