/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <asm_types.h>

__addr_t *stack_init(void *task_entry, void *parameter, __addr_t *stack_addr, void *task_exit)
{
    __addr_t *stk;
    __wk_u8_t i = 0;

    stk  = stack_addr;
    
    *stk = 0x01000000L;                                /* PSR */
    *(--stk) = (unsigned long)task_entry;              /* pc */
    *(--stk) = (unsigned long)task_exit;               /* lr */
    *(--stk) = 0;                                      /* r12 */
    *(--stk) = 0;                                      /* r3 */
    *(--stk) = 0;                                      /* r2 */
    *(--stk) = 0;                                      /* r1 */
    *(--stk) = (unsigned long)parameter;               /* r0 */
    
    /*r4 - r11*/
    for(; i < 8; i++)
        *(--stk) = 0x0badc0de;

    return stk;
}