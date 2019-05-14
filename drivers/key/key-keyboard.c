/*
 * Copyright (C) 2019 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/task.h>
#include <wk/err.h>
#include <wk/delay.h>
#include <init/init.h>
#include <gpio.h>
#include <board.h>

#define KEY_X1 PCout(14)
#define KEY_X2 PAout(4)
#define KEY_X3 PAout(8)
#define KEY_X4 PAout(15)
#define KEY_X5 PBout(14)

#define KEY_Y1  PCin(0)
#define KEY_Y2  PCin(1)
#define KEY_Y3  PCin(2)
#define KEY_Y4  PCin(3)
#define KEY_Y5  PCin(4)
#define KEY_Y6  PCin(5)
#define KEY_Y7  PCin(6)
#define KEY_Y8  PCin(7)
#define KEY_Y9  PCin(8)
#define KEY_Y10 PCin(9)
#define KEY_Y11 PCin(10)
#define KEY_Y12 PCin(11)
#define KEY_Y13 PCin(12)
#define KEY_Y14 PCin(13)

#define KEY_TASK_PRIO       3
#define KEY_TASK_STACK_SIZE 256
#define KEY_TASK_TICK       3

#define KEY_UP   0
#define KEY_DOWN 1

static char *key_name[61] = {
    "esc", "1/!", "2/@", "3/#", "4/$", "5/%%", "6/^", "7/&", "8/*", "9/(",
    "0/)", "-/_", "=/+", "back", "tab", "q", "w", "e", "r", "t", "y",
    "u", "i", "o", "p", "[/{", "]/}", "\\/|", "caps", "a", "s", "d", "f",
    "g", "h", "j", "k", "l", ";/:", "'/\"", "enter", "Lshift", "z", "x",
    "c", "v", "b", "n", "m", ",/<", "./>", "//?", "Rshift", "Lctrl", "win",
    "Lalt", "space", "Ralt", "pn", "Rctrl", "fn"
};

static uint8_t position_index[5][14] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
    {14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27},
    {28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 0, 40},
    {41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 52},
    {53, 54, 55, 0, 0, 56, 0, 0, 0, 57, 58, 59, 0 ,60},
};

struct key_info_t {
    uint8_t type;
    uint8_t val;
};

struct key_info_t key_info[61];

static struct task_struct_t *key_task;

static inline void key_set_x_out(uint8_t x, int val)
{
    switch(x) {
        case 0: KEY_X1 = val; break;
        case 1: KEY_X2 = val; break;
        case 2: KEY_X3 = val; break;
        case 3: KEY_X4 = val; break;
        case 4: KEY_X5 = val; break;
        default:
            pr_err("no this key line(%d)\r\n", x);
            break;
    }

    return;
}

static inline int key_get_y_val(uint8_t y)
{
    switch(y) {
        case 0: return KEY_Y1;
        case 1: return KEY_Y2;
        case 2: return KEY_Y3;
        case 3: return KEY_Y4;
        case 4: return KEY_Y5;
        case 5: return KEY_Y6;
        case 6: return KEY_Y7;
        case 7: return KEY_Y8;
        case 8: return KEY_Y9;
        case 9: return KEY_Y10;
        case 10: return KEY_Y11;
        case 11: return KEY_Y12;
        case 12: return KEY_Y13;
        case 13: return KEY_Y14;
        default:
            pr_err("no this key line(%d)\r\n", y);
            return -1;
    }
}

static void key_task_entry(__maybe_unused void* parameter)
{
    int x, y;
    int num;

    while (1) {
        for (x = 0; x < 5; x++) {
            key_set_x_out(x, KEY_DOWN);
            for (y = 0; y < 14; y ++) {
                if (key_get_y_val(y) == KEY_DOWN)
                    key_info[position_index[x][y]].val = KEY_DOWN;
                else
                    key_info[position_index[x][y]].val = KEY_UP;    
            }
            key_set_x_out(x, KEY_UP);
        }
        delay_msec(5);
        for (x = 0; x < 5; x++) {
            key_set_x_out(x, KEY_DOWN);
            for (y = 0; y < 14; y ++) {
                if (key_get_y_val(y) == KEY_DOWN && key_info[position_index[x][y]].val == KEY_DOWN)
                    key_info[position_index[x][y]].val = KEY_DOWN;
                else
                    key_info[position_index[x][y]].val = KEY_UP;    
            }
            key_set_x_out(x, KEY_UP);
        }

        for (num = 0; num < 61; num++) {
            if (key_info[num].val == KEY_DOWN)
                pr_info("key[\"%s\"] is down\r\n", key_name[key_info[num].type]);
        }
        delay_msec(20);
    }
}

int key_keyboard_init(void)
{
    int i;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_8 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_8 | GPIO_Pin_15);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC, GPIO_Pin_14);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
        GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |
        GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
        GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    key_task = task_create("key", key_task_entry, NULL, KEY_TASK_STACK_SIZE, KEY_TASK_PRIO,
        KEY_TASK_TICK, NULL, NULL);
    if (key_task == NULL) {
        pr_err("creat key task err\n");
        return -1;
    }

    for (i = 0; i < 61; i++)
        key_info[i].type = i;
    
    return 0;
}
task_init(key_keyboard_init);