/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "stm32f10x.h"
#include "arch_clk.h"
#include "arch_dma.h"
#include "arch_gpio.h"
#include "arch_irq.h"
#include "arch_uart.h"

#define UART_LOG_DMA_BUF_SIZE 256

extern struct uart_config_t uart_log_dev;

int board_config_init(void);
int consol_init(void);
void send_to_console(void);

#endif