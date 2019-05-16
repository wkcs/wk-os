#include <wk/init.h>
#include <wk/task.h>

#include "keyboard.h"

#define LED_TASK_PRIO       3
#define LED_TASK_STACK_SIZE 256
#define LED_TASK_TICK       3

static struct task_struct_t *led_task;

static void led_task_entry(__maybe_unused void* parameter)
{
}

int key_led_init(void)
{
    led_task = task_create("led", led_task_entry, NULL, LED_TASK_STACK_SIZE, LED_TASK_PRIO,
        LED_TASK_TICK, NULL, NULL);
    if (led_task == NULL) {
        pr_err("creat led task err\n");
        return -1;
    }
    task_ready(led_task);
    
    return 0;
}
task_init(key_led_init);