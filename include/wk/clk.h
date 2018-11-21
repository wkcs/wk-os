/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __CLK_H__
#define __CLK_H__

#include <wk/kernel.h>

void system_beat_processing(void);
uint32_t get_run_tick(void);

inline uint32_t tick_to_msec(uint32_t tick)
{
    return tick * 1000 / TICK_PER_SECOND;
}

inline uint32_t tick_to_sec(uint32_t tick)
{
    return tick / TICK_PER_SECOND;
}

inline uint32_t msec_to_tick(uint32_t msec)
{
    return msec * TICK_PER_SECOND / 1000;
}

inline uint32_t sec_to_tick(uint32_t sec)
{
    return sec * TICK_PER_SECOND;
}

#endif