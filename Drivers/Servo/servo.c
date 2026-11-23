#include "servo.h"

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

void servo_init(void)
{
    gpio_set_function(SERVO_GPIO, GPIO_FUNC_PWM);

    uint slice = pwm_gpio_to_slice_num(SERVO_GPIO);
    pwm_config config = pwm_get_default_config();

    // 1 MHz counter: each count lasts 1 microsecond.
    float divider = (float)clock_get_hz(clk_sys) / 1000000.0f;
    pwm_config_set_clkdiv(&config, divider);

    // 20,000 microseconds per period = 50 Hz.
    pwm_config_set_wrap(&config, 19999);

    pwm_init(slice, &config, false);
    pwm_set_gpio_level(SERVO_GPIO, SERVO_CLOSED_US);
    pwm_set_enabled(slice, true);
}

void servo_open(void)
{
    pwm_set_gpio_level(SERVO_GPIO, SERVO_OPEN_US);
}

void servo_close(void)
{
    pwm_set_gpio_level(SERVO_GPIO, SERVO_CLOSED_US);
}