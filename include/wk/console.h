/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <wk/kernel.h>

void cmd_buf_deal(char *buf, size_t len);
int console_send_data(char *buf, size_t len);

#endif