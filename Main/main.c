#include <stdint.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "Ambient/ssd1306_i2c.h"
#include "Ambient/bme280_spi.h"
#include "Ambient/ambient.h"
#include "Ultrasonic/ultrasonic.h"
#include "Keypad/keypad.h"
#include "RTC/rtc.h"

#include "pico/cyw43_arch.h"
#include "Servo/servo.h"

#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "drivers.h"
#include <string.h>
#include "pico/rand.h"

const char *generate_password(void)
{
    static char password[7];
    static const char characters[] = "ABCD0123456789*#";

    for (unsigned int i = 0; i < 6; i++) {
        password[i] = characters[get_rand_32() & 0x0F];
    }

    password[6] = '\0';
    return password;
}


int main(void)
{
    stdio_init_all();
    sleep_ms(2000);

    
    // if (cyw43_arch_init() != 0) {
    //     printf("Wi-Fi initialization failed\n");
    //     return 1;
    // }

    // cyw43_arch_enable_sta_mode();

    // printf("Connecting to Wi-Fi...\n");

    // int wifi_result = cyw43_arch_wifi_connect_timeout_ms(
    //     "YOUR_WIFI_NAME",
    //     "YOUR_WIFI_PASSWORD",
    //     CYW43_AUTH_WPA2_AES_PSK,
    //     30000
    // );

    // if (wifi_result != 0) {
    //     printf("Wi-Fi connection failed: %d\n", wifi_result);
    //     return 1;
    // }

    // printf("Wi-Fi connected\n");

    // OLED initialization: I2C0 on GP4 (SDA) and GP5 (SCL).
    i2c_init(i2c0, 100000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    sleep_ms(100);


    //printf("I2C0 initialized for SSD1306 OLED display.\n");
    // BME280 Initialization: Use the SPI bus and pins defined in bme280_spi.h    
    gpio_init(mySPI_CSN);
    SSD1306_initialise();
    gpio_put(mySPI_CSN, 1);
    gpio_set_dir(mySPI_CSN, GPIO_OUT);
    printf("SPI initialized for BME280 sensor.\n");

    spi_init(mySPI, 500000);
    gpio_set_function(mySPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(mySPI_TX, GPIO_FUNC_SPI);
    gpio_set_function(mySPI_RX, GPIO_FUNC_SPI);
    sleep_ms(10);

    // Wait for the BME280 before displaying measurements.
    uint8_t chip_id = 0;
    read_registers(0xD0, &chip_id, 1);
    while (chip_id != 0x60) {
        ClearDisplay();
        WriteString(10, 0, (uint8_t *)"BME280 ERROR");
        UpdateDisplay();
        sleep_ms(1000);
        read_registers(0xD0, &chip_id, 1);
    }
    //printf("BME280 detected, chip ID: 0x%02X\n", chip_id);
    write_register(0xF4, 0x00);  // Sleep while configuring
    read_compensation_parameters();
    write_register(0xF2, 0x01);  // Humidity oversampling x1
    write_register(0xF4, 0x27);  // Temperature and pressure x1, normal mode
    sleep_ms(100);

    // Ultrasonic sensor initialization
    static uint32_t us_distance;
    static uint32_t timer1, timer2;
    const uint32_t us_period = 30; //Period for ultrasonic sensor needs to be at least 20ms or else it may have an error at the rated 2m range
    stdio_init_all();
    us_init();
    us_distance = 0;
    timer1 = timer_read();
    bool user_pres = 0;
    uint32_t door_open_time = 0;

    // Servo initialization
    servo_init(); // Initialize the servo motor
    bool door_status = false; // Placeholder for door status, replace with actual logic to determine door status

    // Keypad initialization
    keypad_init(); 
    unsigned char previous_key = KEYPAD_NO_KEY;
    char password_entered[7] = {0}; // Buffer to store entered password
    char password_correct[] = {'1', '2', '3', '4', '5', '6'};
    unsigned int password_index = 0;
    uint password_uses = 0;
    bool password_status = false; // Password status boolean to track if the password is correct
    bool entering_password = false;
    uint32_t door_opened_at = 0;
    unsigned char key = '0';

    // Real time clock initalization
    ds3231_init(i2c0);
    bool ambient_ok = true;
    int i = 0;
    

    // MAIN LOOP
    while (true) 
    {
        // Get the current distance from the ultrasonic sensor
        timer2 = timer_read();
        if(timer_elapsed_ms(timer1, timer2) >= us_period)
        {
            us_tick(&us_distance);
            //printf("Distance: %d cm\n", us_distance);
            timer1 = timer2;
        }
        user_pres = user_present(us_distance);
        sleep_ms(50);

        ambient_ok = ambient_passes();

        screen_params_update(door_status, ambient_ok, user_pres);
        // Always scan so key releases are detected.
        key = get_keypad_pressedKey(1);

        if (ambient_ok && user_pres)
        {
            int i = 0;
            previous_key = KEYPAD_NO_KEY;

            printf("Password: ");
            fflush(stdout);

            while (i < 6)
            {
                key = get_keypad_pressedKey(1);

                if (key != KEYPAD_NO_KEY &&
                    previous_key == KEYPAD_NO_KEY)
                {
                    password_entered[i++] = (char)key;

                    printf("%c", (int)key);
                    fflush(stdout);
                }

                previous_key = key;
                sleep_ms(10);

                uint32_t distance = us_getdistance();

                if (distance != NO_CAPTURE_READY) {
                    us_distance = distance;
                }

                ambient_ok = ambient_passes();
                user_pres = user_present(us_distance);

                screen_params_update(door_status, ambient_ok, user_pres);

                if (!ambient_ok || !user_pres) {
                    printf("\r\nEntry cancelled\r\n");
                    break;
                }
            }

            password_entered[6] = '\0';
            printf("\r\n");

            password_status =
                strcmp(password_entered, password_correct) == 0;

            if (password_status)
            {
                printf("Password correct\r\n");

                servo_open();
                door_status = true;
                password_uses++;

                screen_params_update(
                    door_status, ambient_ok, user_pres);

                sleep_ms(DOOR_HOLD_MS);

                servo_close();
                door_status = false;

                screen_params_update(
                    door_status, ambient_ok, user_pres);
            }
            else
            {
                printf("Incorrect password entered.\r\n");
            }
        }

        previous_key = key;
        // Close the door when its hold time expires.
        if (door_status && (uint32_t)(time_us_32() - door_opened_at) >= (uint32_t)DOOR_HOLD_MS * 1000u)
        {
            servo_close();
            door_status = false;

            screen_params_update(
                door_status, ambient_ok, user_pres);
        }

        if(password_uses >= 5)
        {
            strcpy(password_correct, generate_password());
            password_uses = 0;
            printf("Password: %s\r\n", password_correct);
        }
        sleep_ms(1000);
        

        //report_status(!door_status, ambient_ok, password_status);

        ds3231_dateTime now;

        // if (ds3231_read_time(&now)) {
        //     printf("%02u:%02u:%02u\r\n",
        //         (unsigned)now.hour,
        //         (unsigned)now.min,
        //         (unsigned)now.sec);
        // } else {
        //     printf("RTC read failed\r\n");
        // }

        //sleep_ms(1000);
    }
}
