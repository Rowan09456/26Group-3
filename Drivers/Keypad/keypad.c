#include "keypad.h"

static uint rows[NUM_ROWS];
static uint cols[NUM_COLS];

// 4x5 Grid Layout matching project specifications
static const char keymap[NUM_ROWS][NUM_COLS] = {
    {'1', '2', '3', 'A', 'E'},
    {'4', '5', '6', 'B', 'F'},
    {'7', '8', '9', 'C', 'G'},
    {'*', '0', '#', 'D', 'H'}
};

void keypad_init(const uint row_pins[NUM_ROWS], const uint col_pins[NUM_COLS]) {
    for (int r = 0; r < NUM_ROWS; r++) {
        rows[r] = row_pins[r];
        gpio_init(rows[r]);
        gpio_set_dir(rows[r], GPIO_OUT);
        gpio_put(rows[r], 1); // Idle HIGH
    }

    for (int c = 0; c < NUM_COLS; c++) {
        cols[c] = col_pins[c];
        gpio_init(cols[c]);
        gpio_set_dir(cols[c], GPIO_IN);
        gpio_pull_up(cols[c]); // Pull-up for active-LOW sensing
    }
}

char keypad_scan(void) {
    for (int r = 0; r < NUM_ROWS; r++) {
        gpio_put(rows[r], 0); // Drive active row LOW
        sleep_us(10);

        for (int c = 0; c < NUM_COLS; c++) {
            if (!gpio_get(cols[c])) { // Active-LOW key press detected
                while (!gpio_get(cols[c])) sleep_ms(10); // Debounce release
                gpio_put(rows[r], 1);
                return keymap[r][c];
            }
        }
        gpio_put(rows[r], 1);
    }
    return '\0'; // No key pressed
}

bool keypad_read_password(char *buffer, size_t max_len) {
    static size_t idx = 0;
    char key = keypad_scan();

    if (key != '\0') {
        if (key == '#') { // '#' acts as ENTER
            buffer[idx] = '\0';
            idx = 0;
            return true;
        } else if (key == '*') { // '*' acts as CLEAR
            idx = 0;
            buffer[0] = '\0';
        } else if (idx < max_len - 1 && idx < PASSWORD_LEN) {
            buffer[idx++] = key;
            buffer[idx] = '\0';
        }
    }
    return false;
}