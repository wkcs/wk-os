/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <board.h>

void board_init(void)
{
    SysTick_Config(SystemCoreClock / TICK_PER_SECOND);
    board_config_init();
}