/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/irq.h>

void (*__irq_handler[128])(void);

int register_irq(int num, void (*callback_func)(void))
{
    if (num >= 0 && num < 128)
        __irq_handler[num] = callback_func;
    else
        return -1;
    
    return 0;
}