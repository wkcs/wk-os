/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/device.h>
#include <wk/mm.h>
#include <wk/err.h>
#include <wk/cpu.h>

int drv_register(struct device_driver *drv)
{
    struct driver_private *p;
    struct device *dev_temp;
    struct bus_type *bus;
    register addr_t level;

    p = wk_alloc(sizeof(struct driver_private), 0, 0);
    if (p == NULL)
        return -ENOMEM;
    drv->p = p;

    INIT_LIST_HEAD(&p->devices_list);
    p->driver = drv;

    bus = drv->bus;
    INIT_LIST_HEAD(&drv->list);

    level = disable_irq_save();

    list_add_tail(&drv->list, &bus->p->drivers_list);

    list_for_each_entry(dev_temp, &bus->p->devices_list, list) {
        if (bus->match(dev_temp, drv) == 0) {
            list_add_tail(&dev_temp->dlist, &drv->p->devices_list);
            dev_temp->driver = drv;
        }
    }

    enable_irq_save(level);
    if (dev_temp->driver != NULL && drv->probe != NULL)
        drv->probe(dev_temp);

    return 0;
}

int drv_unregister(struct device_driver *drv)
{
    struct device *dev_temp;
    register addr_t level;

    level = disable_irq_save();
    if (drv->remove) {
        list_for_each_entry(dev_temp, &drv->p->devices_list, dlist) {
            drv->remove(dev_temp);
            dev_temp->driver = NULL;
        }
    }
    list_del(&drv->list);
    enable_irq_save(level);

    wk_free(drv->p);
    
    return 0;
}

