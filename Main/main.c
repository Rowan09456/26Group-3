#include <stdio.h>
#include "pico/stdlib.h"
#include "Display/display.h"
#include "Ambient/ambient.h"
#include "Ultrasonic/ultrasonic.h"
#include "Reporting/reporting.h"
#include "Keypad/keypad.h"
#include "Servo/servo.h"
#include "ThroughBeam/through_beam.h"

// Define GPIO pin mappings
const uint row_pins[4] = {18, 19, 20, 21};
const uint col_pins[5] = {9, 10, 11, 12, 13};

int main(void) {
    stdio_init_all();

    // 1. Initialize Display & Hardware Drivers
    display_init();
    servo_init(DEFAULT_SERVO_PIN);
    through_beam_init(DEFAULT_THROUGH_BEAM_PIN);
    keypad_init(row_pins, col_pins);
    us_init();

    bool door_status = false;    // false = Closed, true = Open
    bool password_status = false;
    char pass_buffer[PASSWORD_LEN + 1] = {0};

    while (true) {
        bool ambient_ok = ambient_passes();
        bool user_present = false; // Add ultrasonic threshold detection here

        // Read Keypad input
        if (keypad_read_password(pass_buffer, sizeof(pass_buffer))) {
            // Check 6-char password against target (e.g. "123456")
            if (string_compare(pass_buffer, "123456")) { 
                password_status = true;
                door_status = true;
                servo_open_door();
            } else {
                password_status = false;
            }
        }

        // Anti-Pinch Safety Check during door closure
        if (through_beam_is_blocked() && door_status) {
            servo_open_door(); // Re-open door if beam path obstructed
        }

        // Update Display & Modbus/Web Reporting
        display_update_status(door_status, ambient_ok, user_present, password_status);
        report(door_status, ambient_ok, password_status);

        sleep_ms(100);
    }
}