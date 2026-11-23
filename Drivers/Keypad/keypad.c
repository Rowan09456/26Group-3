#include "keypad.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include "hardware/gpio.h"

// Example GPIO assignments. Change to match your wiring.
// These avoid your OLED, BME280, servo, and GP16.
static const uint row_pins[4] = {17, 18, 19, 20};
static const uint col_pins[4] = {21, 22, 26, 27};

// Change these layouts to match the labels on your keypad.
static const unsigned char keypad_1[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

static const unsigned char keypad_2[4][4] = {
    {'7', '8', '9', '/'},
    {'4', '5', '6', '*'},
    {'1', '2', '3', '-'},
    {'C', '0', '=', '+'}
};

void keypad_init(void)
{
    for (uint32_t   i = 0; i < 4; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_IN);
        gpio_pull_up(row_pins[i]);

        gpio_init(col_pins[i]);
        gpio_disable_pulls(col_pins[i]);
        gpio_put(col_pins[i], 0);
        gpio_set_dir(col_pins[i], GPIO_IN);
    }
}

unsigned char get_keypad_pressedKey(unsigned char keypad_select)
{
    if (keypad_select != 1 && keypad_select != 2) {
        return KEYPAD_NO_KEY;
    }

    for (unsigned int col = 0; col < 4; col++) {
        // Activate one column; all others remain high impedance.
        gpio_put(col_pins[col], 0);
        gpio_set_dir(col_pins[col], GPIO_OUT);
        sleep_us(10);

        for (unsigned int row = 0; row < 4; row++) {
            if (!gpio_get(row_pins[row])) {
                sleep_ms(10);  // Confirm press after debounce.

                if (!gpio_get(row_pins[row])) {
                    unsigned char key = keypad_select == 1
                        ? keypad_1[row][col]
                        : keypad_2[row][col];

                    gpio_set_dir(col_pins[col], GPIO_IN);
                    return key;
                }
            }
        }

        gpio_set_dir(col_pins[col], GPIO_IN);
    }

    return KEYPAD_NO_KEY;
}