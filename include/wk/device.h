/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __WK_DEVICE_H__
#define __WK_DEVICE_H__

#include <wk/list.h>
#include <wk/of.h>
#include <wk/mutex.h>
#include <asm/types.h>

struct device;
struct device_private;
struct device_driver;
struct driver_private;
struct subsys_private;
struct bus_type;

struct bus_type {
	const char		*name;

	int (*match)(struct device *dev, struct device_driver *drv);
	int (*probe)(struct device *dev);
	int (*remove)(struct device *dev);
	void (*shutdown)(struct device *dev);

	int (*online)(struct device *dev);
	int (*offline)(struct device *dev);

	int (*suspend)(struct device *dev);
	int (*resume)(struct device *dev);

	struct list_head list;

	struct subsys_private *p;
};

struct subsys_private {
	struct mutex mutex;

	struct list_head devices_list;
	struct list_head drivers_list;
	struct bus_type *bus;
};

struct device_driver {
	const char		*name;
	struct bus_type		*bus;

	const struct of_device_id	*of_match_table;

	int (*probe) (struct device *dev);
	int (*remove) (struct device *dev);
	void (*shutdown) (struct device *dev);
	int (*suspend) (struct device *dev);
	int (*resume) (struct device *dev);

	struct list_head list;

	struct driver_private *p;
};

struct driver_private {
	struct list_head devices_list;
	struct device_driver *driver;
};

struct device {
	struct device		*parent;

	struct device_private	*p;

	const char		*init_name; /* initial name of the device */

	struct mutex		mutex;	/* mutex to synchronize calls to
					 * its driver.
					 */

	struct bus_type	*bus;		/* type of bus device is on */
	struct device_driver *driver;	/* which driver has allocated this
					   device */
	void		*platform_data;	/* Platform specific data, device
					   core doesn't touch it */
	void		*driver_data;	/* Driver data, set and get with
					   dev_set/get_drvdata */

	struct device_node	*of_node; /* associated device tree node */

	struct list_head	devres_head;
	struct list_head list;
	struct list_head dlist;
};

struct device_private {
	struct list_head children_list;
	struct list_head parent_list;
	struct device *device;
};

#endif