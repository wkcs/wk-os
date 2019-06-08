/*
 * Copyright (C) 2019 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __I2C_H__
#define __I2C_H__

#include <wk/kernel.h>

int i2c_core_init(void);
int __i2c_init(void);
int __i2c_remove(void);

#endif