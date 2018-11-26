/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/clk.h>
#include <wk/irq.h>
#include <board.h>

void SysTick_Handler(void)
{
    /* enter interrupt */
    wk_interrupt_enter();

    if (kernel_running)
        system_beat_processing();

    /* leave interrupt */
    wk_interrupt_leave();
}

void board_init(void)
{
    /*uint32_t reload;

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	reload = SystemCoreClock/8000000;					   
	reload *= 1000000/TICK_PER_SECOND;			   

	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk; 
	SysTick->LOAD=reload; 
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;*/
    SysTick_Config( SystemCoreClock / TICK_PER_SECOND );
    board_config_init();
}