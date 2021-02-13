/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/clk.h>
#include <wk/task.h>
#include <wk/sch.h>
#include <wk/irq.h>
#include <wk/cpu.h>
#include <wk/delay.h>

void delay_sec(uint16_t sec)
{
    delay_msec(sec * 1000);
}

void delay_msec(uint16_t msec)
{
    uint16_t temp;

    if (kernel_running && get_irq_level() == 0) {
        temp = 1000 / TICK_PER_SECOND;
        if (msec >= temp)
            task_sleep(msec_to_tick(msec / temp));
        msec = msec % temp;
        if (msec == 0)
            return;
    }

    delay_usec(msec * 1000);
}

void delay_usec(uint32_t usec)
{
    register addr_t level;

    level = disable_irq_save();
    cpu_delay_usec(usec);
    enable_irq_save(level);
}