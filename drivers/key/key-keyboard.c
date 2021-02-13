/*
 * Copyright (C) 2019 胡启航 <Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/task.h>
#include <wk/err.h>
#include <wk/delay.h>
#include <init/init.h>
#include <lib/string.h>
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

#define KEY_TASK_PRIO       6
#define KEY_TASK_STACK_SIZE 1024
#define KEY_TASK_TICK       3

#define KEY_UP   0
#define KEY_DOWN 1

#define LSHIFT 41
#define RSHIFT 52
#define LCTRL  53
#define LWIN   54
#define LALT   55
#define RALT   57
#define RCTRL  59
#define FN     60
#define PN     58

#define DEF_LAYER 0
#define FN_LAYER  1
#define PN_LATER  2

#if 0
static char *key_name[61] = {
    "esc", "1/!", "2/@", "3/#", "4/$", "5/%%", "6/^", "7/&", "8/*", "9/(",
    "0/)", "-/_", "=/+", "back", "tab", "q", "w", "e", "r", "t", "y",
    "u", "i", "o", "p", "[/{", "]/}", "\\/|", "caps", "a", "s", "d", "f",
    "g", "h", "j", "k", "l", ";/:", "'/\"", "enter", "Lshift", "z", "x",
    "c", "v", "b", "n", "m", ",/<", "./>", "//?", "Rshift", "Lctrl", "win",
    "Lalt", "space", "Ralt", "pn", "Rctrl", "fn"
};
#endif

static int position_index[5][14] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
    {14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27},
    {28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, -1, 40},
    {41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, 52},
    {53, 54, 55, -1, -1, 56, -1, -1, -1, 57, 58, 59, -1 ,60},
};

struct key_info_t {
    uint8_t type;
    uint8_t val;
};

static struct key_info_t key_info[3][61] = {
    {   /* default layer */
        {0x29, 0}, // esc
        {0x1e, 0}, // 1
        {0x1f, 0}, // 2
        {0x20, 0}, // 3
        {0x21, 0}, // 4
        {0x22, 0}, // 5
        {0x23, 0}, // 6
        {0x24, 0}, // 7
        {0x25, 0}, // 8
        {0x26, 0}, // 9
        {0x27, 0}, // 0
        {0x2d, 0}, // -
        {0x2e, 0}, // =
        {0x2a, 0}, // backspack
        {0x2b, 0}, // tab
        {0x14, 0}, // q
        {0x1a, 0}, // w
        {0x08, 0}, // e
        {0x15, 0}, // r
        {0x17, 0}, // t
        {0x1c, 0}, // y
        {0x18, 0}, // u
        {0x0c, 0}, // i
        {0x12, 0}, // o
        {0x13, 0}, // p
        {0x2f, 0}, // [
        {0x30, 0}, // ]
        {0x31, 0}, // |
        {0x39, 0}, // caps
        {0x04, 0}, // a
        {0x16, 0}, // s
        {0x07, 0}, // d
        {0x09, 0}, // f
        {0x0a, 0}, // g
        {0x0b, 0}, // h
        {0x0d, 0}, // j
        {0x0e, 0}, // k
        {0x0f, 0}, // l
        {0x33, 0}, // ;
        {0x34, 0}, // '
        {0x28, 0}, // enter
        {0x00, 0}, // lshift
        {0x1d, 0}, // z
        {0x1b, 0}, // x
        {0x06, 0}, // c
        {0x19, 0}, // v
        {0x05, 0}, // b
        {0x11, 0}, // n
        {0x10, 0}, // m
        {0x36, 0}, // ,
        {0x37, 0}, // .
        {0x38, 0}, // /
        {0x00, 0}, // rshift
        {0x00, 0}, // lctrl
        {0x00, 0}, // lwin
        {0x00, 0}, // lalt
        {0x2c, 0}, // space
        {0x00, 0}, // ralt
        {0x00, 0}, // pn
        {0x00, 0}, // rctrl
        {0x00, 0}, // fn
    }, {  /* fn layer */
        {0x32, 0}, // ~
        {0x3a, 0}, // F1
        {0x3b, 0}, // F2
        {0x3c, 0}, // F3
        {0x3d, 0}, // F4
        {0x3e, 0}, // F5
        {0x3f, 0}, // F6
        {0x40, 0}, // F7
        {0x41, 0}, // F8
        {0x42, 0}, // F9
        {0x43, 0}, // F10
        {0x44, 0}, // F11
        {0x45, 0}, // F12
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x4c, 0}, // delete
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x52, 0}, // up
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x50, 0}, // left
        {0x51, 0}, // down
        {0x4f, 0}, // right
        {0x00, 0},
    }, {  /* pn layer */
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
        {0x00, 0},
    }
};

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

extern void hid_write_test(const void *buffer, size_t size);

static void key_task_entry(__maybe_unused void* parameter)
{
    int x, y;
    int num;
    uint8_t data[8];
    uint8_t data_len;
    uint8_t layer = 0;
    uint8_t data_buf = 0;
    bool last_is_down = false;
    bool key_down;

    while (1) {
        memset(data, 0, 8);
        data_len = 0;
        key_down = false;
        for (x = 0; x < 5; x++) {
            key_set_x_out(x, KEY_DOWN);
            for (y = 0; y < 14; y ++) {
                if (position_index[x][y] < 0)
                    continue;
                if (key_get_y_val(y) == KEY_DOWN) {
                    key_info[0][position_index[x][y]].val = KEY_DOWN;
                    if (position_index[x][y] == FN && layer == 0)
                        layer = 1;
                    else if (position_index[x][y] == PN && layer == 0)
                        layer = 2;
                    else
                        layer = 0;
                } else {
                    key_info[0][position_index[x][y]].val = KEY_UP;
                }
            }
            key_set_x_out(x, KEY_UP);
        }
        delay_msec(5);
        for (x = 0; x < 5; x++) {
            key_set_x_out(x, KEY_DOWN);
            for (y = 0; y < 14; y ++) {
                if (position_index[x][y] < 0)
                    continue;
                if (key_get_y_val(y) == KEY_DOWN && key_info[0][position_index[x][y]].val == KEY_DOWN)
                    key_info[0][position_index[x][y]].val = KEY_DOWN;
                else
                    key_info[0][position_index[x][y]].val = KEY_UP;    
            }
            key_set_x_out(x, KEY_UP);
        }
        for (num = 0; num < 61; num++) {
            if (key_info[0][num].val == KEY_DOWN) {
                key_down = true;
                switch (num) {
                    case LSHIFT:
                        if (layer == 0) {
                            data[0] |= (1 << 1);
                        } else {
                            if (data_buf & (uint8_t)(1 << 1)) {
                                data[0] |= (1 << 1);
                            } else {
                                if (key_info[layer][num].type != 0) {
                                    if (data_len < 6) {
                                        data[2 + data_len] = key_info[layer][num].type;
                                        data_len++;
                                    }
                                }
                                layer = 0;
                            }
                        }
                        break;
                    case RSHIFT:
                        if (layer == 0) {
                           data[0] |= (1 << 5);
                        } else {
                            if (data_buf & (uint8_t)(1 << 5)) {
                                data[0] |= (1 << 5);
                            } else {
                                if (key_info[layer][num].type != 0) {
                                    if (data_len < 6) {
                                        data[2 + data_len] = key_info[layer][num].type;
                                        data_len++;
                                    }
                                }
                                layer = 0;
                            }
                        }
                        break;
                    case LCTRL:
                        if (layer == 0) {
                           data[0] |= 1;
                        } else {
                            if (data_buf & (uint8_t)1) {
                                data[0] |= 1;
                            } else {
                                if (key_info[layer][num].type != 0) {
                                    if (data_len < 6) {
                                        data[2 + data_len] = key_info[layer][num].type;
                                        data_len++;
                                    }
                                }
                                layer = 0;
                            }
                        }
                        break;
                    case LWIN:
                        if (layer == 0) {
                           data[0] |= (1 << 3);
                        } else {
                            if (data_buf & (uint8_t)(1 << 3)) {
                                data[0] |= (1 << 3);
                            } else {
                                if (key_info[layer][num].type != 0) {
                                    if (data_len < 6) {
                                        data[2 + data_len] = key_info[layer][num].type;
                                        data_len++;
                                    }
                                }
                                layer = 0;
                            }
                        }
                        break;
                    case LALT:
                        if (layer == 0) {
                           data[0] |= (1 << 2);
                        } else {
                            if (data_buf & (uint8_t)(1 << 2)) {
                                data[0] |= (1 << 2);
                            } else {
                                if (key_info[layer][num].type != 0) {
                                    if (data_len < 6) {
                                        data[2 + data_len] = key_info[layer][num].type;
                                        data_len++;
                                    }
                                }
                                layer = 0;
                            }
                        }
                        break;
                    case RALT:
                        if (layer == 0) {
                           data[0] |= (1 << 6);
                        } else {
                            if (data_buf & (uint8_t)(1 << 6)) {
                                data[0] |= (1 << 6);
                            } else {
                                if (key_info[layer][num].type != 0) {
                                    if (data_len < 6) {
                                        data[2 + data_len] = key_info[layer][num].type;
                                        data_len++;
                                    }
                                }
                                layer = 0;
                            }
                        }
                        break;
                    case RCTRL:
                        if (layer == 0) {
                           data[0] |= (1 << 4);
                        } else {
                            if (data_buf & (uint8_t)(1 << 4)) {
                                data[0] |= (1 << 4);
                            } else {
                                if (key_info[layer][num].type != 0) {
                                    if (data_len < 6) {
                                        data[2 + data_len] = key_info[layer][num].type;
                                        data_len++;
                                    }
                                }
                                layer = 0;
                            }
                        }
                        break;
                    case FN:
                        if (layer == 2) {
                            if (key_info[layer][FN].type != 0) {
                                if (data_len < 6) {
                                    data[2 + data_len] = key_info[layer][FN].type;
                                    data_len++;
                                }
                            }
                            layer = 0;
                        }
                        break;
                    case PN:
                        if (layer == 1) {
                            if (key_info[layer][PN].type != 0) {
                                if (data_len < 6) {
                                    data[2 + data_len] = key_info[layer][PN].type;
                                    data_len++;
                                }
                            }
                            layer = 0;
                        }
                        break;
                    default:
                        if (key_info[layer][num].type != 0) {
                            if (data_len < 6) {
                                data[2 + data_len] = key_info[layer][num].type;
                                data_len++;
                            }
                        }
                        layer = 0;
                    break;
                }
            }
        }
        if (key_down) {
            hid_write_test(data, 8);
            last_is_down = true;
        } else if (last_is_down) {
            hid_write_test(data, 8);
            last_is_down = false;
        }
        data_buf = data[0];
        delay_msec(20);
    }
}

int key_keyboard_init(void)
{
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
    task_ready(key_task);
    
    return 0;
}
task_init(key_keyboard_init);