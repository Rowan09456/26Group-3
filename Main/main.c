#include <stdint.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "Ambient/ssd1306_i2c.h"
#include "Ambient/bme280_spi.h"
#include "Ambient/ambient.h"
#include "Ultrasonic/ultrasonic.h"
#include "Reporting/reporting.h"

void screen_params_update(bool door_status, bool ambient_ok, bool user_present);

int main(void)
{
    // OLED: I2C0 on GP4 (SDA) and GP5 (SCL).
    i2c_init(i2c0, 100000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    sleep_ms(100);

    SSD1306_initialise();

    // BME280: use the SPI bus and pins defined in bme280_spi.h.
    gpio_init(mySPI_CSN);
    gpio_put(mySPI_CSN, 1);
    gpio_set_dir(mySPI_CSN, GPIO_OUT);

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

    bool door_status = true; // Placeholder for door status, replace with actual logic to determine door status
    bool password_status = true; // Placeholder for password status, replace with actual logic to determine password status

  
    
    

    while (true) {

        // bool ambient_ok = ambient_passes();
        // ClearDisplay();
        // if (ambient_ok) {
        //     WriteString(10, 0, (uint8_t *)"Ambient: OK");
        // } else {
        //     WriteString(10, 0, (uint8_t *)"Ambient: NOK");
        // }
        // UpdateDisplay();

        timer2 = timer_read();
        if(timer_elapsed_ms(timer1, timer2) >= us_period){
            us_tick(&us_distance);
            printf("Distance: %d cm\n", us_distance);
            timer1 = timer2;
        }

        screen_params_update(door_status, ambient_passes(), user_present(us_distance));
        report(door_status, ambient_passes(), password_status);

        sleep_ms(1000);
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