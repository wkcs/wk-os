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
#include <asm/types.h>

struct device;
struct device_private;
struct device_driver;
struct driver_private;
struct subsys_private;
struct bus_type;
struct device_node;

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

	struct subsys_private *p;
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

	struct driver_private *p;
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
};

#endif