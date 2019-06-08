#include <wk/task.h>
#include <wk/cpu.h>
#include <wk/delay.h>
#include <wk/err.h>
#include <init/init.h>

#include "keyboard.h"

#define LED_TASK_PRIO       16
#define LED_TASK_STACK_SIZE 256
#define LED_TASK_TICK       3

static uint8_t display_buf[15][14];

static struct task_struct_t *led_task;

#if 0
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
#endif

#define RGB_DIV     15
#define RGB_DIV_NUM (255 / RGB_DIV)
#define RRG_NUM     (RGB_DIV_NUM * 3)
static void led_task_entry(__maybe_unused void* parameter)
{
    uint8_t x, y;
    uint8_t num = 0, num_tmp;
    uint8_t r, g, b;
    int tmp;

    while (1) {
        for (x = 0; x < 14; x++) {
            num_tmp = num + x;
            if (num_tmp >= RRG_NUM)
                num_tmp = num_tmp - RRG_NUM;
            tmp = num_tmp / RGB_DIV_NUM;
            if (tmp == 0) {
                r = 255 - (num_tmp % RGB_DIV_NUM) * 5;
                g = (num_tmp % RGB_DIV_NUM) * 5;
                b = 0;
            } else if (tmp == 1) {
                r = 0;
                g = 255 - (num_tmp % RGB_DIV_NUM) * 5;
                b = (num_tmp % RGB_DIV_NUM) * 5;
            } else {
                r = (num_tmp % RGB_DIV_NUM) * 5;
                g = 0;
                b = 255 - (num_tmp % RGB_DIV_NUM) * 5;
            }
            for (y = 0; y < 5; y++) {
                display_buf[y * 3][x] = r;
                display_buf[y * 3 + 1][x] = g;
                display_buf[y * 3 + 2][x] = b;
            }
        }
        num++;
        if (num >= RRG_NUM)
            num = 0;
        led_write_display_buf((uint8_t *)display_buf);
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