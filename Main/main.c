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

void screen_params_update(bool door_status, bool ambient_ok, bool user_present);

int main(void)
{
    stdio_init_all();
    sleep_ms(2000);

    servo_init(); // Initialize the servo motor

    printf("Servo opening\n");
    servo_open();
    sleep_ms(2000);

    printf("Servo closing\n");
    servo_close();
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

    //OLED: I2C0 on GP4 (SDA) and GP5 (SCL).
    i2c_init(i2c0, 100000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    sleep_ms(100);
    printf("I2C0 initialized for SSD1306 OLED display.\n");

    SSD1306_initialise();

    //BME280: use the SPI bus and pins defined in bme280_spi.h.
    gpio_init(mySPI_CSN);
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

    printf("BME280 detected, chip ID: 0x%02X\n", chip_id);

    write_register(0xF4, 0x00);  // Sleep while configuring
    read_compensation_parameters();
    write_register(0xF2, 0x01);  // Humidity oversampling x1
    write_register(0xF4, 0x27);  // Temperature and pressure x1, normal mode
    sleep_ms(100);

    // Ultrasonic sensor
    static uint32_t us_distance;
    static uint32_t timer1, timer2;
    const uint32_t us_period = 30; //Period for ultrasonic sensor needs to be at least 20ms or else it may have an error at the rated 2m range
    stdio_init_all();
    us_init();
    us_distance = 0;
    timer1 = timer_read();

    bool door_status = false; // Placeholder for door status, replace with actual logic to determine door status
    bool password_status = true; // Placeholder for password status, replace with actual logic to determine password status

    keypad_init(); // Initialize the keypad
    unsigned char previous_key = KEYPAD_NO_KEY;

    ds3231_init(i2c0);
    
    while (true) {

        bool ambient_ok = ambient_passes();
        ClearDisplay();
        if (ambient_ok) {
            WriteString(10, 0, (uint8_t *)"Ambient: OK");
        } else {
            WriteString(10, 0, (uint8_t *)"Ambient: NOK");
        }
        UpdateDisplay();

        timer2 = timer_read();
        if(timer_elapsed_ms(timer1, timer2) >= us_period){
            us_tick(&us_distance);
            //printf("Distance: %d cm\n", us_distance);
            timer1 = timer2;
        }
        //printf("Distance: %d cm\n", us_distance);
        screen_params_update(
        door_status,
        ambient_ok,
        user_present(us_distance)
        );

        //printf("Reporting: door_closed=%d ambient_ok=%d password_ok=%d\n", !door_status, ambient_ok, password_status);

        //report_status(!door_status, ambient_ok, password_status);

        // Check the through beam sensor
        // if (through_beam_blocked()) 
        // {
        //     printf("Through beam sensor is blocked.\n");
        // }
        // else
        // {
        //     printf("Through beam sensor is clear.\n");
        // }

        // if(door_status)
        // {
        //     servo_close();
        // }
        // else
        // {
        //     servo_open();
        // }
        // door_status = !door_status; // Toggle door status for demonstration purposes





    

        unsigned char key = get_keypad_pressedKey(1);

        // Print once per press.
        if (key != KEYPAD_NO_KEY && key != previous_key) {
            printf("Pressed: %c\r\n", key);

            if (key == '1') {
                // Action for key 1.
            }
        }

        previous_key = key;
        sleep_ms(10);
        ds3231_dateTime now;

        if (ds3231_read_time(&now)) {
            printf("%02u:%02u:%02u\r\n",
                (unsigned)now.hour,
                (unsigned)now.min,
                (unsigned)now.sec);
        } else {
            printf("RTC read failed\r\n");
        }

        //sleep_ms(1000);
    }
}


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

