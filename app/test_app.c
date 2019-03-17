#include <wk/kernel.h>
#include <wk/task.h>
#include <wk/delay.h>
#include <init/init.h>

static void test_app_entry(__maybe_unused void* parameter)
{
    while (1)
    {
        pr_info("test app running\r\n");
        delay_sec(1);
    }
}

int test_app_task_init(void)
{
    struct task_struct_t *test_app_task;

    test_app_task = task_create("test_app", test_app_entry, NULL, 512, 35, 3, NULL, NULL);
    task_ready(test_app_task);

    return 0;

}
task_init(test_app_task_init);