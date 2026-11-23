#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include "pico/stdlib.h"

// Initialize OLED hardware on I2C0 (GP4 = SDA, GP5 = SCL)
void display_init(void);

// Update main operational status screen
void display_update_status(bool door_open, bool ambient_ok, bool user_present, bool password_ok);

// Display password typing line (e.g. "Pass: ******")
void display_show_password_entry(const char *typed_buffer);

// Display custom notification message
void display_show_message(const char *line1, const char *line2);

// Clear screen contents
void display_clear(void);

#endif // DISPLAY_H