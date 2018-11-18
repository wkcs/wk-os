/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 *
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include "delay.h"
#include "led.h"

#include <wk/mm.h>
#include <wk/kernel.h>
#include <lib/stdio.h>
#include <wk/printk.h>
#include <wk/log.h>

#include <board.h>

int main(void)
{
    uint32_t i = 0;

    mm_pool_init();
    delay_init();
    board_config_init();
    log_server_init();
    LED_Init();

    printk("wkcs:%s[%d] write log\r\n", __func__, __LINE__);
    while(1) {
        i++;
        delay_ms(10);
        if (i == 50) {
            LED0 = !LED0;
            i = 0;
            printk("wkcs:%s[%d] write log\r\n", __func__, __LINE__);
        }
    }
}