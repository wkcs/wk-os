/*
 * Copyright (C) 2018-2019 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/device.h>
#include <wk/err.h>
#include <wk/cpu.h>
#include <wk/irq.h>
#include <wk/sch.h>
#include <lib/string.h>


/*int dev_register(struct device *dev)
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
}*/

LIST_HEAD(sys_device_list);

struct device *device_alloc(mm_flag_t flag, wk_pid_t pid)
{
    struct device *dev;

    dev = wk_alloc(sizeof(*dev), flag, pid);
    if (dev == NULL) {
        pr_err("alloc device error\r\n");
        return NULL;
    }
    memset((void *)dev, 0, sizeof(*dev));
    INIT_LIST_HEAD(&dev->list);
    dev->ref_count = 0;
    mutex_init(dev->mutex);

    return dev;
}

int device_free(struct device *dev)
{
    if (dev->ref_count != 0) {
        pr_err("device \"%s\" is busy\r\n", dev->name);
        return -EIO;
    }

    wk_free((void *)dev);

    return 0;
}

int device_init(struct device *dev)
{
    if (dev == NULL) {
        pr_err("%s[%d]:dev addr is NULL\r\n", __func__, __LINE__);
        return -EPERM;
    }

    INIT_LIST_HEAD(&dev->list);
    dev->ref_count = 0;
    mutex_init(dev->mutex);

    return 0;
}

void device_put(struct device *dev)
{
    if (dev->ref_count > 0)
        dev->ref_count--;
}

void device_inc(struct device *dev)
{
    if (dev->ref_count < U32_MAX)
        dev->ref_count++;
}

int device_register(struct device *dev)
{
    register addr_t level;
    struct device *dev_tmp;

    if (dev == NULL) {
        pr_err("%s[%d]:dev addr is NULL\r\n", __func__, __LINE__);
        return -EPERM;
    }

    dev_tmp = device_find_by_name(dev->name);
    if (dev_tmp != NULL) {
        pr_err("device \"%s\" already exists\r\n", dev->name);
        return -EALREADY;
    }

    level = disable_irq_save();
    list_add(&dev->list, &sys_device_list);
    enable_irq_save(level);

    return 0;
}

int device_unregister(struct device *dev)
{
    register addr_t level;

    if (dev == NULL) {
        pr_err("%s[%d]:dev addr is NULL\r\n", __func__, __LINE__);
        return -EPERM;
    }

    level = disable_irq_save();
    list_del(&dev->list);
    enable_irq_save(level);

    return 0;
}

struct device *device_find_by_name(const char *name)
{
    struct device *dev;
    register addr_t level;

    if (name == NULL) {
        pr_err("%s[%d]:name addr is NULL\r\n", __func__, __LINE__);
        return NULL;
    }

    level = disable_irq_save();
    list_for_each_entry(dev, &sys_device_list, list) {
        if (!strcmp(name, dev->name)) {
            enable_irq_save(level);
            return dev;
        }
    }
    enable_irq_save(level);

    return NULL;
}

