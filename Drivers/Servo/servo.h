#ifndef SERVO_H
#define SERVO_H

#include <stdbool.h>
#include "pico/stdlib.h"

#define DEFAULT_SERVO_PIN 15

void servo_init(uint gpio_pin);
void servo_set_angle(float angle_deg);
void servo_open_door(void);
void servo_close_door(void);

#endif // SERVO_H