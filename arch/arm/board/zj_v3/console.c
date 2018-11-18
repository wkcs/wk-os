/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/log.h>

#include "board.h"

char log_buf[UART_LOG_DMA_BUF_SIZE];
static bool dma_transport = false;

int consol_init(void)
{
    uart_log_dev.dma_config->init_type.DMA_MemoryBaseAddr = (uint32_t)log_buf;

    return uart_config(&uart_log_dev);
}

void send_to_console(void)
{
    size_t len;

    if (kernel_running) {
        if (!dma_transport) {
            len = read_log(log_buf, UART_LOG_DMA_BUF_SIZE);
            if (!len)
                return;
            dma_transport = true;
            DMA_Cmd(uart_log_dev.dma_config->ch, DISABLE );   
            DMA_SetCurrDataCounter(uart_log_dev.dma_config->ch, len);
            DMA_Cmd(uart_log_dev.dma_config->ch, ENABLE);
        }
    } else {
        len = read_log(log_buf, UART_LOG_DMA_BUF_SIZE);
        if (!len)
            return;
        usart_send(log_buf, len);
    }
}

void DMA1_Channel4_IRQHandler(void)
{
    size_t len;

    DMA_ClearITPendingBit(DMA1_IT_TC4);
    len = read_log(log_buf, UART_LOG_DMA_BUF_SIZE);
    if (!len) {
        dma_transport = false;
        return;
    }
    DMA_Cmd(uart_log_dev.dma_config->ch, DISABLE );
 	DMA_SetCurrDataCounter(uart_log_dev.dma_config->ch, len);
 	DMA_Cmd(uart_log_dev.dma_config->ch, ENABLE);
}