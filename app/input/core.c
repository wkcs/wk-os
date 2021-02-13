/*
 * Copyright (C) 2019 胡启航 <Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/input.h>
#include <wk/mm.h>
#include <wk/err.h>

static LIST_HEAD(input_dev_list);

int input_dev_register(struct input_dev *dev, struct list_head event_list)
{
    int ret;

    dev->input_sem = sem_alloc(1, 0);
    if (dev->input_sem == NULL) {
        pr_err("%s: alloc sem err\r\n", __func__);
        return -ENOMEM;
    }

    INIT_LIST_HEAD(&dev->event_list);
    INIT_LIST_HEAD(&dev->listen_task_list);
    INIT_LIST_HEAD(&dev->list);
    dev->event_num = 0;

    list_add(&dev->list, &input_dev_list);

    return 0;
}

struct key_event *input_alloc_event(uint8_t code)
{
    struct key_event *event;

    event = wk_alloc(sizeof(struct key_event, 0, 0));
    if (event == NULL) {
      pr_err("%s: alloc event err\r\n", __func__);
      return -ENOMEM;
    }

    event->code = code;
    INIT_LIST_HEAD(&event->list);

    return event;
}

int input_activation_event(struct input_dev *dev, struct key_event *event)
{
    if (dev->event_num >= EVENT_NUM_MAX) {
        pr_err("Activation events reach the maximum(%d)\r\n", EVENT_NUM_MAX);
        return -1;
    } else {
        list_add(&event->list, &dev->event_list);
        dev->event_num++;
    }

    return 0;
}

int input_register_listen_task(struct input_dev *dev, struct task_struct_t *task)
{
    list_add(task->listen_list, &dev->listen_list);
    return 0;
}

int input_sync(struct input_dev *dev)
{
  struct task_struct_t *task_tmp;
  if (list_empty(&dev->listen_list)) {
    pr_err("input dev no listen task\r\n");
    return -1;
    }

    list_for_each_entry(list_temp, &system_timer_list, list) {

    }
}


