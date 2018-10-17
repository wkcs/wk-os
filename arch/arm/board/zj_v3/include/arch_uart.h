/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __ARCH_UART_H__
#define __ARCH_UART_H__

#include <wk/kernel.h>

#include "stm32f10x.h" 
#include "arch_clk.h"
#include "arch_dma.h"
#include "arch_irq.h"
#include "arch_gpio.h"

struct uart_config_t {
    struct clk_config_t *clk_config;
    struct dma_config_t *dma_config;
    struct irq_config_t *irq_config;
    uint32_t irq_type;
    struct gpio_config_t (*gpio_config)[];
    uint32_t gpio_group_num;
    USART_TypeDef *uart;
    USART_InitTypeDef init_type;
};

int uart_config(struct uart_config_t *config);
int usart_send(char *ptr, int len);
#endif


