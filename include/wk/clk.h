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

extern uint64_t run_tick;

void system_beat_processing(void);

inline uint32_t tick_to_msec(uint32_t tick)
{
    if (TICK_PER_SECOND < 1000) {
        pr_warning("tick min is %d\r\n", TICK_PER_SECOND / 1000);
        return 1;
    }

    return tick * 1000 / TICK_PER_SECOND;
}

inline uint64_t tick_to_msec_64(uint64_t tick)
{
    if (TICK_PER_SECOND < 1000) {
        pr_warning("tick min is %d\r\n", TICK_PER_SECOND / 1000);
        return 1;
    }
    
    return tick * 1000 / TICK_PER_SECOND;
}

inline uint32_t tick_to_sec(uint32_t tick)
{
    return tick / TICK_PER_SECOND;
}

inline uint64_t tick_to_sec_64(uint64_t tick)
{
    return tick / TICK_PER_SECOND;
}

inline uint32_t msec_to_tick(uint32_t msec)
{
    if (TICK_PER_SECOND < 1000) {
        pr_warning("msec min is %d\r\n", 1000/ TICK_PER_SECOND);
        return 1;
    }

    return msec * TICK_PER_SECOND / 1000;
}

inline uint64_t msec_to_tick_64(uint64_t msec)
{
    if (TICK_PER_SECOND < 1000) {
        pr_warning("msec min is %d\r\n", 1000/ TICK_PER_SECOND);
        return 1;
    }

    return msec * TICK_PER_SECOND / 1000;
}

inline uint32_t sec_to_tick(uint32_t sec)
{
    return sec * TICK_PER_SECOND;
}

inline uint64_t sec_to_tick_64(uint64_t sec)
{
    return sec * TICK_PER_SECOND;
}

/*
 * 获取kernel当前运行时间片
 */
inline uint64_t get_run_tick(void)
{
    return run_tick;
}

#endif