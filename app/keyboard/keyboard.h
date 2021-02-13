#include <wk/kernel.h>

#define BACKLIGHT_ON  1
#define BACKLIGHT_OFF 0

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
void pwm_set_led_off(void);
void pwm_set_led_data(uint8_t buf[14]);
void led_write_display_buf(uint8_t *buf);
void led_backlight_on_off(uint8_t on);
void led_backlight_set_lum(uint8_t lum);