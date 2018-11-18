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

void mm_dump(void)
{
    struct mm_list_t *list_temp;
    uint32_t i = 0;

    list_for_each_entry(list_temp, mm_pool_data.head, list) {
        i++;
        pr_info("block[%d]----addr = %p\r\n", i, list_temp);
        pr_info("         |---size = %d\r\n", list_temp->size);
        pr_info("         -----num = %d\r\n", list_temp->block_num);
    }
}

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

    if (!addr) {
        pr_err("%s[%d]:the addr to be free is NULL %d\r\n", __func__, __LINE__);
        return -1;
    }

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

    //mm_dump();

    return addr;
}

int wk_free(void *addr)
{
    register addr_t level;
    int status = 0;

    level = disable_irq_save();

    status = __mm_free(addr);

    enable_irq_save(level);

    //mm_dump();   

    return status;
}

void wk_mm_set_pid(wk_pid_t pid, void *addr)
{

}

#define STACK_GROW_DOWN

void *stack_alloc(size_t stack_size)
{
    register addr_t level;
    void *addr;

    if (stack_size % 4) {
        pr_err("%s[%d]:stack_size must be an integer multiple of %d\r\n", __func__, __LINE__, MM_ALIGN);
        return NULL;
    }

    level = disable_irq_save();

    addr = __mm_alloc(stack_size, 0, 0);

    enable_irq_save(level);

    //mm_dump();

#ifdef STACK_GROW_UP
    return addr;
#else
    return (void *)((addr_t)addr + stack_size);
#endif
}

int stack_free(size_t stack_size, void *addr)
{
    register addr_t level;
    int status = 0;

    if (stack_size % 4) {
        pr_err("%s[%d]:stack_size must be an integer multiple of %d\r\n", __func__, __LINE__, MM_ALIGN);
        return -1;
    }

    if (!addr) {
        pr_err("%s[%d]:the addr to be free is NULL %d\r\n", __func__, __LINE__);
        return -1;
    }

#ifdef STACK_GROW_DOWN
    if ((addr_t)addr < stack_size) {
        pr_err("%s[%d]:the addr is error %d\r\n", __func__, __LINE__);
        return -1;
    }
    addr = (void *)((addr_t)addr - stack_size);
#endif

    level = disable_irq_save();

    status = __mm_free(addr);

    enable_irq_save(level);

    return status;
}
