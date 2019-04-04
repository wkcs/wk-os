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

int dev_register(struct device *dev)
{
    struct device_private *p;
    struct device_driver *drv_temp;
    struct bus_type *bus;
    register addr_t level;

    p = wk_alloc(sizeof(struct device_private), 0, 0);
    if (p == NULL)
        return -ENOMEM;
    dev->p = p;

    INIT_LIST_HEAD(&p->children_list);
    INIT_LIST_HEAD(&p->parent_list);
    p->device = dev;

    if (dev->parent) {
        level = disable_irq_save();
        list_add(&p->parent_list, &dev->parent->p->children_list);
        enable_irq_save(level);
    }

    bus = dev->bus;

    mutex_init(dev->mutex);
    INIT_LIST_HEAD(&dev->devres_head);
    INIT_LIST_HEAD(&dev->list);
    INIT_LIST_HEAD(&dev->dlist);

    level = disable_irq_save();

    list_add_tail(&dev->list, &bus->p->devices_list);

    list_for_each_entry(drv_temp, &bus->p->drivers_list, list) {
        if (bus->match(dev, drv_temp) == 0) {
            list_add_tail(&dev->dlist, &drv_temp->p->devices_list);
            dev->driver = drv_temp;
        }
    }

    enable_irq_save(level);
    if (dev->driver != NULL && dev->driver->probe != NULL)
        drv_temp->probe(dev);

    return 0;
}

int dev_unregister(struct device *dev)
{
    struct device *dev_temp;
    struct device_private *p_temp;
    register addr_t level;

    if (dev->driver->remove)
        dev->driver->remove(dev);

    list_del(&dev->dlist);
    list_del(&dev->list);

    level = disable_irq_save();

    if (!list_empty(&dev->p->children_list)){
        list_for_each_entry(p_temp, &dev->p->children_list, parent_list) {
            dev_temp = p_temp->device;
            dev_unregister(dev_temp);
        }
    }
    list_del(&dev->p->parent_list);

    enable_irq_save(level);

    wk_free(dev->p);
    
    return 0;
}

