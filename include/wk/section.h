/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __SECTION_H__
#define __SECTION_H__

#define __init __attribute__((section(".kernel_init")))
#define __init_task __attribute__((section(".init_task")))
#define __cmd __attribute__((section(".cmd_list")))

#endif