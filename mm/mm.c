/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 *
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <wk/mm.h>
#include <wk/list.h>
#include <wk/pid.h>
#include <wk/config.h>

static struct mm_list_t *__find_block(size_t size)
{
    int num = 0;
    struct mm_list_t *list_temp;

    //list_temp = list_first_entry(mm_pool_data.head, typeof(*list_temp), list);
    list_for_each_entry(list_temp, mm_pool_data.head, list) {
        if (list_temp->block_num == 0) {
            num++;
            if (size <= (MM_BLOCK_SIZE * num - sizeof(struct mm_list_t))) {
                list_temp = (struct mm_list_t *)((addr_t)list_temp - MM_BLOCK_SIZE * (num - 1));
                list_temp->block_num = num;
                return list_temp;
            }
        } else
            num = 0;
    }

    return NULL;
}

static void *__mm_alloc(size_t size, mm_flag_t flag, wk_pid_t pid)
{
    struct mm_list_t *list_temp;

    list_temp = __find_block(size);
    if (list_temp == NULL)
        return NULL;
    list_temp->pid = pid;
    list_temp->size = size;
    list_temp->flag = flag;
    list_temp->list.next = &((struct mm_list_t *)((addr_t)list_temp + MM_BLOCK_SIZE * list_temp->block_num))->list;
    ((struct mm_list_t *)((addr_t)list_temp + MM_BLOCK_SIZE * list_temp->block_num))->list.prev = &list_temp->list;

    return (void *)((addr_t)list_temp + sizeof(struct mm_list_t));
}
/* zzr
 * 释放内存
 * @param 需要释放地址的指针
 * @return int
 */
static int __mm_free(void *addr)
{
    struct list_head *head_main, *head_new;
    struct mm_list_t *list;
    size_t num;

    LIST_HEAD(head);

    list = (struct mm_list_t *)((addr_t)addr - sizeof(struct mm_list_t));

    head_main = &list->list;

    num = list->block_num;

    list->pid = 0;
    list->size = 0;
    list->block_num = 0;
    list->flag = 0;

    head_new = __mm_init((addr_t)list + MM_BLOCK_SIZE, (addr_t)list + num * MM_BLOCK_SIZE);
    if (head_new == NULL) {
        return -1;
    }

    list_add(&head, head_new);

    list_splice(&head, head_main);

    return 0;
}

void *wk_alloc(size_t size, mm_flag_t flag, wk_pid_t pid)
{
    register addr_t level;
    void *addr;

    level = disable_irq_save();

    addr = __mm_alloc(size, flag, pid);

    enable_irq_save(level);

    return addr;
}

int wk_free(void *addr)
{
    register addr_t level;
    int status = 0;

    level = disable_irq_save();

    status = __mm_free(addr);

    enable_irq_save(level);   

    return status;
}