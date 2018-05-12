/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __WK_LIST_H__
#define __WK_LIST_H__

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};



#endif