/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 *
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/mm.h>
#include <wk/config.h>

extern addr_t __mm_pool_start, __mm_pool_end;

struct mm_pool_data_t mm_pool_data;

/*
 * 检查内存池容量，对齐，等信息
 *
 * 返回值：0->内存池不符合要求，其他值为内存池大小
 */
size_t check_mm_pool(void)
{
    /*
     *检查内存池起始地址对齐是否符合要求，
     *如不符合，则修改起始地址使其符合要求
     */
    mm_pool_data.mm_pool_start = (addr_t)(&__mm_pool_start);
    if ((addr_t)(&__mm_pool_start) % MM_ALIGN != 0)
        mm_pool_data.mm_pool_start = mm_pool_data.mm_pool_start + MM_ALIGN - ((mm_pool_data.mm_pool_start) % MM_ALIGN);

    mm_pool_data.block_num = (size_t)(((addr_t)(&__mm_pool_end) - mm_pool_data.mm_pool_start) / MM_BLOCK_SIZE);
    mm_pool_data.size = mm_pool_data.block_num * MM_BLOCK_SIZE;
    if (mm_pool_data.size < MM_POOL_MIN_SIZE)
        return 0;
    mm_pool_data.mm_pool_end = mm_pool_data.mm_pool_start + mm_pool_data.size;
    return mm_pool_data.size;
}