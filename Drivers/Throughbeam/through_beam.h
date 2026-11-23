#ifndef THROUGH_BEAM_H
#define THROUGH_BEAM_H

#include <stdbool.h>
#include "pico/stdlib.h"

#define DEFAULT_THROUGH_BEAM_PIN 14

void through_beam_init(uint gpio_pin);
bool through_beam_is_blocked(void);

#endif // THROUGH_BEAM_H