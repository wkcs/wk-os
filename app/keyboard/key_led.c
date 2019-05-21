#include <wk/task.h>
#include <wk/cpu.h>
#include <wk/delay.h>
#include <wk/err.h>
#include <init/init.h>

#include "keyboard.h"

#define LED_TASK_PRIO       3
#define LED_TASK_STACK_SIZE 256
#define LED_TASK_TICK       3

static uint8_t display_buf[15][14];

static struct task_struct_t *led_task;

static uint8_t led_test_buf[30][3] = {
	{ 255,   0,   0 },
	{ 230,  30,   0 },
	{ 205,  55,   0 },
	{ 180,  80,   0 },
	{ 155, 105,   0 },
	{ 130, 130,   0 },
	{ 105, 155,   0 },
	{  80, 180,   0 },
	{  55, 205,   0 },
	{  30, 230,   0 },
	{   0, 255,   0 },
	{   0, 230,  30 },
	{   0, 205,  55 },
	{   0, 180,  80 },
	{   0, 155, 105 },
	{   0, 130, 130 },
	{   0, 105, 155 },
	{   0,  80, 180 },
	{   0,  55, 205 },
	{   0,  30, 230 },
	{   0,   0, 255 },
	{  30,   0, 230 },
	{  55,   0, 205 },
	{  80,   0, 180 },
	{ 105,   0, 155 },
	{ 130,   0, 130 },
	{ 155,   0, 105 },
	{ 180,   0,  80 },
	{ 205,   0,  55 },
	{ 230,   0,  30 },
};
static void led_task_entry(__maybe_unused void* parameter)
{
    static uint8_t m = 0;
	uint8_t i, n, u;
	addr_t level;

    while (1) {
		for (u = 0; u < 14; u++) {
			n = m + u;
			if (n >= 30)
				n = n - 30;
			for (i = 0; i < 5; i++) {
				display_buf[i * 3][u] = led_test_buf[n][0];
				display_buf[i * 3 + 1][u] = led_test_buf[n][1];
				display_buf[i * 3 + 2][u] = led_test_buf[n][2];
                level = disable_irq_save();
                led_write_display_buf((uint8_t *)display_buf);
				enable_irq_save(level);
			}
		}
		m++;
		if (m >= 30)
			m = 0;
		delay_msec(100);
    }
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