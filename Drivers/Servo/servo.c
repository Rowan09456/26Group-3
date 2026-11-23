#include "servo.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

static uint servo_gpio_pin;
static uint servo_slice;
static uint servo_chan;

// 50 Hz PWM period: 150 MHz / (80.0 * 37500) = 50 Hz (20 ms period)
#define PWM_DIV  80.0f
#define PWM_TOP  37499

void servo_init(uint gpio_pin) {
    servo_gpio_pin = gpio_pin;
    gpio_set_function(servo_gpio_pin, GPIO_FUNC_PWM);
    
    servo_slice = pwm_gpio_to_slice_num(servo_gpio_pin);
    servo_chan  = pwm_gpio_to_channel(servo_gpio_pin);

    pwm_set_clkdiv(servo_slice, PWM_DIV);
    pwm_set_wrap(servo_slice, PWM_TOP);
    
    // Default: Door closed (0 degrees)
    servo_close_door();
    pwm_set_enabled(servo_slice, true);
}

void servo_set_angle(float angle_deg) {
    if (angle_deg < 0.0f) angle_deg = 0.0f;
    if (angle_deg > 180.0f) angle_deg = 180.0f;

    // Pulse width: 1.0 ms (0 deg) to 2.0 ms (180 deg)
    // Duty count = 1875 + (angle / 180.0) * 1875
    uint16_t level = (uint16_t)(1875 + (angle_deg / 180.0f) * 1875);
    pwm_set_chan_level(servo_slice, servo_chan, level);
}

void servo_open_door(void) {
    servo_set_angle(90.0f); // 90 degree open angle
}

void servo_close_door(void) {
    servo_set_angle(0.0f);  // 0 degree closed angle
}