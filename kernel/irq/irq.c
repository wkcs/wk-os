/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/irq.h>
#include <wk/cpu.h>

volatile uint8_t interrupt_nest;

inline void wk_interrupt_enter(void)
{
    register addr_t level;
    
    level = disable_irq_save();
    interrupt_nest++;
    enable_irq_save(level);
}

inline void wk_interrupt_leave(void)
{
    register addr_t level;
    
    level = disable_irq_save();
    interrupt_nest--;
    enable_irq_save(level);
}

inline uint8_t get_irq_level(void)
{
    return interrupt_nest;
}