#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdbool.h>
#include <stddef.h>
#include "pico/stdlib.h"

#define NUM_ROWS 4
#define NUM_COLS 5
#define PASSWORD_LEN 6

void keypad_init(const uint row_pins[NUM_ROWS], const uint col_pins[NUM_COLS]);
char keypad_scan(void);
bool keypad_read_password(char *buffer, size_t max_len);

#endif // KEYPAD_H