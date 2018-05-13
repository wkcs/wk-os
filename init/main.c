/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 *
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

#include <wk/mm.h>
#include <wk/mm_core.h>
#include <wk/kernel.h>

int main(void)
{
    struct mm_list_t *list_temp;
    int i = 0;
    void *test_addr, *test_addr2, *test_addr3;

    delay_init();
    uart_init(115200);
    LED_Init();
    printf("start\r\n");
    mm_pool_init();
    printf("init ok\r\n");
    list_for_each_entry(list_temp, mm_pool_data.head, list) {
        i++;
        printf("block[%d]----addr = %p\r\n", i, list_temp);
        printf("         |---size = %d\r\n", list_temp->size);
        printf("         -----num = %d\r\n", list_temp->block_num);
    }
    test_addr = __mm_alloc(500, 0, 25);
    test_addr2 = __mm_alloc(138, 0, 25);
    test_addr3 = __mm_alloc(500, 0, 25);
    __mm_free(test_addr2);
    test_addr = __mm_alloc(900, 0, 25);
    test_addr = __mm_alloc(12, 0, 25);
    test_addr = __mm_alloc(8, 0, 25);
    test_addr = __mm_alloc(26, 0, 25);
    test_addr2 = __mm_alloc(128, 0, 25);
    test_addr = __mm_alloc(800, 0, 25);
    i = 0;
    list_for_each_entry(list_temp, mm_pool_data.head, list) {
        i++;
        printf("block[%d]----addr = %p\r\n", i, list_temp);
        printf("         |---size = %d\r\n", list_temp->size);
        printf("         -----num = %d\r\n", list_temp->block_num);
    }
    __mm_free(test_addr2);
    test_addr = __mm_alloc(500, 0, 25);
    printf("\r\ntest_addr = %p\r\n\r\n", test_addr);
    i = 0;
    list_for_each_entry(list_temp, mm_pool_data.head, list) {
        i++;
        printf("block[%d]----addr = %p\r\n", i, list_temp);
        printf("         |---size = %d\r\n", list_temp->size);
        printf("         -----num = %d\r\n", list_temp->block_num);
    }
    /*__mm_free(test_addr);
    i = 0;
    list_for_each_entry(list_temp, mm_pool_data.head, list) {
        i++;
        printf("block[%d]----addr = %p\r\n", i, list_temp);
        printf("         |---size = %d\r\n", list_temp->size);
        printf("         -----num = %d\r\n", list_temp->block_num);
    }*/
    while (1) {
        LED0 = 0;
        LED1 = 1;
        delay_ms(300);
        LED0 = 1;
        LED1 = 0;
        delay_ms(300);
        printf("Hello wk-os\r\n");
    }
}