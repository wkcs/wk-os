/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/log.h>
#include <wk/irq.h>
#include <wk/cpu.h>
#include <wk/console.h>

#include "board.h"

char log_buf[UART_LOG_DMA_BUF_SIZE];
static bool dma_transport = false;

extern uint8_t interrupt_nest;

int consol_init(void)
{
    uart_log_dev.dma_config->init_type.DMA_MemoryBaseAddr = (uint32_t)log_buf;

    return uart_config(&uart_log_dev);
}

void send_to_console(void)
{
    register addr_t level;
    size_t len;

    if (kernel_running && interrupt_nest == 0) {
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
        level = disable_irq_save();
        usart_send(log_buf, len);
        enable_irq_save(level);
    }
}

void DMA1_Channel4_IRQHandler(void)
{
    size_t len;

    wk_interrupt_enter();

    DMA_ClearITPendingBit(DMA1_IT_TC4);
    len = read_log(log_buf, UART_LOG_DMA_BUF_SIZE);
    if (!len) {
        dma_transport = false;
        wk_interrupt_leave();
        return;
    }
    DMA_Cmd(uart_log_dev.dma_config->ch, DISABLE );
 	DMA_SetCurrDataCounter(uart_log_dev.dma_config->ch, len);
 	DMA_Cmd(uart_log_dev.dma_config->ch, ENABLE);

    wk_interrupt_leave();
}

static char cmd_buf[256];
static uint8_t cmd_num;
void USART1_IRQHandler(void)
{
    char res;
    wk_interrupt_enter();
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        res = USART_ReceiveData(USART1);
        if (res == '\r') {
            cmd_buf[cmd_num + 1] = 0;
            cmd_buf_deal(cmd_buf, cmd_num);
            cmd_num = 0;
            usart_send("\r\n", 2);
        } else if (res != 0x09 && res != 0x1b) {
            if (res == '\b') {
                if (cmd_num > 0) {
                    cmd_num--;
                    cmd_buf[cmd_num] = 0;
                    usart_send(&res, 1);
                    usart_send(" ", 1);
                    usart_send(&res, 1);
                }
            } else {
                cmd_buf[cmd_num] = res;
                cmd_num++;
                usart_send(&res, 1);
            }
        }
        if (cmd_num == 255) {
            cmd_buf_deal(cmd_buf, cmd_num);
            cmd_num = 0;
            usart_send("\r\n", 2);
        }
    }

    wk_interrupt_leave();
}

int console_send_data(char *buf, size_t len)
{
    return usart_send(buf, len);
}
