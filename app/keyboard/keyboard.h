#include <wk/kernel.h>

enum led_ch {
    LED1,
    LED2,
    LED3,
    LED4,
    LED5,
    LED6,
    LED7,
    LED8,
    LED9,
    LED10,
    LED11,
    LED12,
    LED13,
    LED14,
};

void pwm_set_val(enum led_ch ch, uint8_t val);
