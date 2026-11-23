#include "display.h"
#include "hardware/i2c.h"
#include "Ambient/ssd1306_i2c.h"

void display_init(void) {
    // I2C0 on GP4 (SDA) and GP5 (SCL) matching partner's main.c setup
    i2c_init(i2c0, 100000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    sleep_ms(100);

    SSD1306_initialise();
    ClearDisplay();
    WriteString(10, 20, (uint8_t *)"System Init...");
    UpdateDisplay();
}

void display_update_status(bool door_open, bool ambient_ok, bool user_present, bool password_ok) {
    ClearDisplay();

    // Line 1: Ambient Status
    if (ambient_ok) {
        WriteString(0, 0, (uint8_t *)"Ambient:  OK");
    } else {
        WriteString(0, 0, (uint8_t *)"Ambient:  NOK");
    }

    // Line 2: Door Status
    if (door_open) {
        WriteString(0, 16, (uint8_t *)"Door:     OPEN");
    } else {
        WriteString(0, 16, (uint8_t *)"Door:     CLOSED");
    }

    // Line 3: User Presence Status
    if (user_present) {
        WriteString(0, 32, (uint8_t *)"User:     PRESENT");
    } else {
        WriteString(0, 32, (uint8_t *)"User:     AWAY");
    }

    // Line 4: Password Verification Status
    if (password_ok) {
        WriteString(0, 48, (uint8_t *)"Auth:     GRANTED");
    } else {
        WriteString(0, 48, (uint8_t *)"Auth:     DENIED");
    }

    UpdateDisplay();
}

void display_show_password_entry(const char *typed_buffer) {
    ClearDisplay();
    WriteString(0, 10, (uint8_t *)"Enter Password:");
    WriteString(0, 30, (uint8_t *)typed_buffer);
    UpdateDisplay();
}

void display_show_message(const char *line1, const char *line2) {
    ClearDisplay();
    if (line1) WriteString(0, 16, (uint8_t *)line1);
    if (line2) WriteString(0, 36, (uint8_t *)line2);
    UpdateDisplay();
}

void display_clear(void) {
    ClearDisplay();
    UpdateDisplay();
}