/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/mutex.h>
#include <wk/task.h>
#include <wk/sch.h>
#include <wk/cpu.h>

/*设置互斥体的拥有者*/
static inline void mutex_set_owner(struct mutex *lock, struct task_struct_t *task)
{
    lock->owner = task;
}

/*清除互斥体的拥有者*/
static inline void mutex_clear_owner(struct mutex *lock)
{
    lock->owner = NULL;
}

/*初始化一个互斥体*/
void __mutex_init(struct mutex *lock, const char *name)
{
    lock->count = 1;
    INIT_LIST_HEAD(&lock->wait_list);
    mutex_clear_owner(lock);
#ifdef CONFIG_DEBUG_MUTEXES
    lock->name = name;
#endif

    /*这句话什么也不做，是为了防止在没有定义CONFIG_DEBUG_MUTEXES是编译器产生警告*/
    debug_mutex_init(lock, name);
}

/*给互斥体加锁*/
void mutex_lock(struct mutex *lock)
{
    register addr_t level;

    level = disable_irq_save();
    if (lock->count == 1) {
        lock->count = 0;
        mutex_set_owner(lock, get_current_task());
        enable_irq_save(level);
        return;
    } else {
        struct task_struct_t *task = get_current_task();
        struct task_struct_t *task_temp;

        list_del(&task->list);

        if (list_empty(&lock->wait_list))
            list_add_tail(&task->list, &lock->wait_list);
        else {
            list_for_each_entry(task_temp, &lock->wait_list, list) {
                if (task->current_priority > task_temp->current_priority)
                    break;
            }
            list_add_tail(&task->list, &task_temp->list);
        }

        if (task->current_priority < lock->owner->current_priority)
            task_ctrl(lock->owner, CMD_TASK_SET_CURR_PRIO, &task->current_priority);

        task->status = TASK_WAIT;
        enable_irq_save(level);

        timer_stop(&task->timer);
        switch_task();
    }
}

/*尝试给一个互斥体加锁，失败则立即返回*/
int mutex_try_lock(struct mutex *lock)
{
    register addr_t level;

    level = disable_irq_save();
    if (lock->count == 1) {
        lock->count = 0;
        mutex_set_owner(lock, get_current_task());
        enable_irq_save(level);
        return 0;
    } else {
        enable_irq_save(level);
        return -1;
    }
}

/*给互斥体解锁*/
void mutex_unlock(struct mutex *lock)
{
    register addr_t level;

    if (lock->owner != get_current_task())
        return;
    if (lock->count != 0)
        return;

    level = disable_irq_save();

    if (list_empty(&lock->wait_list)) {
        lock->count = 1;
        task_ctrl(lock->owner, CMD_TASK_SET_CURR_PRIO, &lock->owner->init_priority);
        mutex_clear_owner(lock);
        enable_irq_save(level);
        return;
    }

    struct task_struct_t *task_temp;
    task_temp = list_entry(lock->wait_list.next, struct task_struct_t, list);
    task_ctrl(lock->owner, CMD_TASK_SET_CURR_PRIO, &lock->owner->init_priority);
    mutex_set_owner(lock, task_temp);
    enable_irq_save(level);

    task_resume(task_temp);
    switch_task();
}

