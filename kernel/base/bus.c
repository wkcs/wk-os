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

static LIST_HEAD(bus_list);

int bus_register(struct bus_type *bus)
{
    struct subsys_private *p;
    register addr_t level;

    p = wk_alloc(sizeof(struct subsys_private), 0, 0);
    if (p == NULL)
        return -ENOMEM;
    bus->p = p;

    mutex_init(p->mutex);
    INIT_LIST_HEAD(&p->devices_list);
    INIT_LIST_HEAD(&p->drivers_list);
    p->bus = bus;

    level = disable_irq_save();
    list_add_tail(&bus->list, &bus_list);
    enable_irq_save(level);

    return 0;
}

int bus_unregister(struct bus_type *bus)
{
    register addr_t level;

    level = disable_irq_save();
    list_del(&bus->list);
    enable_irq_save(level);

    wk_free(bus->p);
    wk_free(bus);

    return 0;
}