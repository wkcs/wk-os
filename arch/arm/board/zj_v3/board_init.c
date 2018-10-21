/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/clk.h>

void SysTick_Handler(void)
{
    /* enter interrupt */
    wk_interrupt_enter();

    system_beat_processing();

    /* leave interrupt */
    wk_interrupt_leave();
}