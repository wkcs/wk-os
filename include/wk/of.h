/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __WK_DT_H__
#define __WK_DT_H__

#include <asm/types.h>
#include <wk/list.h>

struct property {
	char	*name;
	int	length;
	void	*value;
	struct property *next;
	unsigned long _flags;
};

struct device_node {
    const char *name;
	const char *type;
	struct	property *properties;
	struct	device_node *parent;
	struct	device_node *child;
	unsigned long _flags;
	struct list_head list;
};

struct of_device_id {
    char	name[32];
	char	type[32];
	char	compatible[128];
	const void *data;
};

#endif