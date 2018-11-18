/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 *
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include "arch_uart.h"  

static inline void usart_send_blocking(uint8_t data)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);	
	USART_SendData(USART1, data);
}

int usart_send(char *ptr, int len)
{
    int i = 0;
	
    while (*ptr && (i < len))
    {
        usart_send_blocking(*ptr);

        if (*ptr == '\n')
        {
            usart_send_blocking('\r');
        }

        i++;
        ptr++;
    }

    return i;
}


int uart_config(struct uart_config_t *config)
{
    uint32_t num;

    if (config == NULL)
        return 0;
	
	clk_enable_all(config->clk_config);
#ifdef UART_DMA
    dma_config(config->dma_config);
#endif

    for (num = 0; num < config->gpio_group_num; num++)
	    gpio_config(&(*config->gpio_config)[num]);

	USART_Init(config->uart, &config->init_type);
	USART_Cmd(config->uart, ENABLE);

    if (config->irq_config) {
        irq_config(config->irq_config);
        USART_ITConfig(config->uart, config->irq_type, ENABLE);
    }

#ifdef UART_DMA
    if (config->dma_config) {
        USART_DMACmd(config->uart, config->dma_tx_rx, ENABLE);
    }
#endif
    
    return 0;
}

