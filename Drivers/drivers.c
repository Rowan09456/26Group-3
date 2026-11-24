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
#include <string.h>



void screen_params_update(bool door_status, bool ambient_ok, bool user_present) 
{
    ClearDisplay();
    if (ambient_ok) 
    {
        WriteString(10, 0, (uint8_t *)"Ambient: OK");
    } else 
    {
        WriteString(10, 0, (uint8_t *)"Ambient: NOK");
    }

    if (door_status) 
    {
        WriteString(10, 20, (uint8_t *)"Door: Open");
    } else 
    {
        WriteString(10, 20, (uint8_t *)"Door: Closed");
    }

    if (user_present) 
    {
        WriteString(10, 40, (uint8_t *)"User: Present");
    } else 
    {
        WriteString(10, 40, (uint8_t *)"User: Not Present");
    }
    UpdateDisplay();
}

// void init_all(void)
// {
//     stdio_init_all();
//     sleep_ms(2000);


//     // if (cyw43_arch_init() != 0) {
//     //     printf("Wi-Fi initialization failed\n");
//     //     return 1;
//     // }

//     // cyw43_arch_enable_sta_mode();

//     // printf("Connecting to Wi-Fi...\n");

//     // int wifi_result = cyw43_arch_wifi_connect_timeout_ms(
//     //     "YOUR_WIFI_NAME",
//     //     "YOUR_WIFI_PASSWORD",
//     //     CYW43_AUTH_WPA2_AES_PSK,
//     //     30000
//     // );

//     // if (wifi_result != 0) {
//     //     printf("Wi-Fi connection failed: %d\n", wifi_result);
//     //     return 1;
//     // }

//     // printf("Wi-Fi connected\n");

//     // OLED initialization: I2C0 on GP4 (SDA) and GP5 (SCL).
//     i2c_init(i2c0, 100000);
//     gpio_set_function(4, GPIO_FUNC_I2C);
//     gpio_set_function(5, GPIO_FUNC_I2C);
//     gpio_pull_up(4);
//     gpio_pull_up(5);
//     sleep_ms(100);
//     //printf("I2C0 initialized for SSD1306 OLED display.\n");
//     SSD1306_initialise();

//     // BME280 Initialization: Use the SPI bus and pins defined in bme280_spi.h    gpio_init(mySPI_CSN);
//     gpio_put(mySPI_CSN, 1);
//     gpio_set_dir(mySPI_CSN, GPIO_OUT);
//     printf("SPI initialized for BME280 sensor.\n");

//     spi_init(mySPI, 500000);
//     gpio_set_function(mySPI_SCK, GPIO_FUNC_SPI);
//     gpio_set_function(mySPI_TX, GPIO_FUNC_SPI);
//     gpio_set_function(mySPI_RX, GPIO_FUNC_SPI);
//     sleep_ms(10);

//     // Wait for the BME280 before displaying measurements.
//     uint8_t chip_id = 0;
//     read_registers(0xD0, &chip_id, 1);
//     while (chip_id != 0x60) {
//         ClearDisplay();
//         WriteString(10, 0, (uint8_t *)"BME280 ERROR");
//         UpdateDisplay();
//         sleep_ms(1000);
//         read_registers(0xD0, &chip_id, 1);
//     }
//     //printf("BME280 detected, chip ID: 0x%02X\n", chip_id);
//     write_register(0xF4, 0x00);  // Sleep while configuring
//     read_compensation_parameters();
//     write_register(0xF2, 0x01);  // Humidity oversampling x1
//     write_register(0xF4, 0x27);  // Temperature and pressure x1, normal mode
//     sleep_ms(100);

//     // Ultrasonic sensor initialization
//     static uint32_t us_distance;
//     static uint32_t timer1, timer2;
//     const uint32_t us_period = 30; //Period for ultrasonic sensor needs to be at least 20ms or else it may have an error at the rated 2m range
//     stdio_init_all();
//     us_init();
//     us_distance = 0;
//     timer1 = timer_read();

//     // Servo initialization
//     servo_init(); // Initialize the servo motor
//     bool door_status = false; // Placeholder for door status, replace with actual logic to determine door status

//     // Keypad initialization
//     keypad_init(); 
//     unsigned char previous_key = KEYPAD_NO_KEY;
//     char password_entered[6] = {0}; // Buffer to store entered password
//     char password_correct[6] = {'1', '2', '3', '4', '5', '6'}; // Example correct password
//     uint password_uses = 0;
//     bool password_status = false; // Password status boolean to track if the password is correct

//     // Real time clock initalization
//     ds3231_init(i2c0);
//     bool ambient_ok = true;
// }

