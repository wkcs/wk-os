/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>

enum err_t {
    NORMAL = 0,
};

struct err_info_t {
    char *func;
    char *info;
    char *parent_func;
    addr_t *stack_addr;
};