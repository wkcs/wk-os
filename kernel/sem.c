/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/sem.h>
#include <wk/mm.h>
#include <wk/err.h>
#include <wk/task.h>
#include <wk/sch.h>
#include <wk/timer.h>

/*分配一个信号量*/
sem_t *sem_alloc(uint8_t init_num, wk_pid_t pid)
{
    sem_t *sem;

    sem = wk_alloc(sizeof(sem_t), 0, pid);
    if (sem == NULL) {
        pr_err("%s: alloc sem memory error\r\n", __func__);
        return NULL;
    }

    sem->count = init_num;
    INIT_LIST_HEAD(&sem->wait_list);

    return sem;
}

static void __sem_get(sem_t *sem)
{
    register addr_t level;
    struct task_struct_t *task = get_current_task();
    struct task_struct_t *task_temp;

    level = disable_irq_save();
    list_del(&task->wait_list);
    list_for_each_entry(task_temp, &sem->wait_list, wait_list) {
        if (task->current_priority > task_temp->current_priority)
            break;
    }
    list_add_tail(&task->wait_list, &task_temp->wait_list);
    enable_irq_save(level);
}

void sem_get(sem_t *sem)
{
    register addr_t level;
    struct task_struct_t *task = get_current_task();

    if (sem->count > 0) {
        level = disable_irq_save();
        sem->count--;
        enable_irq_save(level);
    } else {
        __sem_get(sem);
        task_hang(task);
        switch_task();
    }
}

int sem_get_timeout(sem_t *sem, uint32_t timedout)
{
    register addr_t level;
    struct task_struct_t *task = get_current_task();

    if (sem->count > 0) {
        level = disable_irq_save();
        sem->count--;
        enable_irq_save(level);
        return 0;
    }

    __sem_get(sem);
    if (timedout == 0) {
        return -EBUSY;
    }
    timer_ctrl(&task->timer, CMD_TIMER_SET_TICK, &timedout);
    timer_start(&task->timer);
    switch_task();

    if (task->flag == -ETIMEDOUT) {
        task->flag = 0;
        return -ETIMEDOUT;
    }

    return 0;
}

void sem_send(sem_t *sem, uint8_t num)
{
    register addr_t level;
    struct task_struct_t *task_temp;

    WK_WARNING(num > 0);

    level = disable_irq_save();
    list_for_each_entry(task_temp, &sem->wait_list, wait_list) {
        num--;
        task_resume(task_temp);
        if (num == 0)
            break;
    }
    sem->count += num;
    enable_irq_save(level);

    switch_task();
}

