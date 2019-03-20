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
#include <lib/string.h>

/*分配一个信号量*/
sem_t *sem_alloc(uint8_t init_num, wk_pid_t pid)
{
    sem_t *sem;

    sem = wk_alloc(sizeof(sem_t), 0, pid);
    if (sem == NULL) {
        pr_err("%s: alloc sem memory error\r\n", __func__);
        return NULL;
    }

    mutex_init(sem->sem_lock);

    sem->count = init_num;
    INIT_LIST_HEAD(&sem->wait_list);

    return sem;
}

void sem_get(sem_t *sem)
{
    mutex_lock(&sem->sem_lock);
    if (sem->count > 0) {
        sem->count--;
        mutex_unlock(&sem->sem_lock);
    } else {
        struct task_struct_t *task = get_current_task();
        struct task_struct_t *task_temp;

        list_del(&task->list);

        if (list_empty(&sem->wait_list))
            list_add_tail(&task->list, &sem->wait_list);
        else {
            list_for_each_entry(task_temp, &sem->wait_list, list) {
                if (task->current_priority > task_temp->current_priority)
                    break;
            }
            list_add_tail(&task->list, &task_temp->list);
        }

        task->status = TASK_WAIT;
        mutex_unlock(&sem->sem_lock);

        timer_stop(&task->timer);
        switch_task();
    }
}

void sem_send(sem_t *sem, uint8_t num)
{
    struct task_struct_t *task_temp;

    WK_WARNING(num > 0);

    mutex_lock(&sem->sem_lock);
    list_for_each_entry(task_temp, &sem->wait_list, list) {
        num--;
        task_resume(task_temp);
        if (num == 0)
            break;
    }
    sem->count += num;
    mutex_unlock(&sem->sem_lock);

    switch_task();
}
