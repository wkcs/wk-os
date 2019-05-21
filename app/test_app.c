#include <wk/kernel.h>
#include <wk/task.h>
#include <wk/delay.h>
#include <i2c.h>
#include <init/init.h>
#include <drivers/usb_device.h>
#include "arch_usb.h"

static void test_app_entry(__maybe_unused void* parameter)
{
    //__i2c_remove();
    while (1)
    {
        pr_info("test app running\r\n");
        delay_sec(1);
    }
}

int test_app_task_init(void)
{
    struct task_struct_t *test_app_task;

    test_app_task = task_create("test_app", test_app_entry, NULL, 512, 20, 3, NULL, NULL);
    task_ready(test_app_task);

    //usbd_hid_class_register();
    usbd_winusb_class_register();
    stm_usbd_register();
    //i2c_core_init();
    //__i2c_init();
    return 0;

}
task_init(test_app_task_init);