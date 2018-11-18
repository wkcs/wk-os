/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/pid.h>
#include <wk/mm.h>

wk_pid_t pid_alloc(addr_t addr)
{
    if (addr > mm_pool_data.mm_pool_start + sizeof(struct mm_list_t))
        return (wk_pid_t)((addr - mm_pool_data.mm_pool_start - sizeof(struct mm_list_t)) << 2);
    else
        return 0;
}
