#include <stdint.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "Ambient/ssd1306_i2c.h"
#include "Ambient/bme280_spi.h"
#include "Ambient/ambient.h"

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
    bool ambient_ok = ambient_passes();

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

    while (true) {
        int32_t raw_humidity = 0;
        int32_t raw_pressure = 0;
        int32_t raw_temperature = 0;

        bme280_read_raw(&raw_humidity, &raw_pressure, &raw_temperature);

        // Temperature compensation sets t_fine for the other two readings.
        double temperature_c = compensate_temp(raw_temperature) / 100.0;
        double pressure_kpa = compensate_pressure(raw_pressure) / 1000.0;
        double humidity_pct = compensate_humidity(raw_humidity) / 1024.0;

        char hum[16];
        char pres[16];
        char temp[16];

        snprintf(hum, sizeof hum, "%.1f %%", humidity_pct);
        snprintf(pres, sizeof pres, "%.1f KPA", pressure_kpa);
        snprintf(temp, sizeof temp, "%.1f C", temperature_c);

        ClearDisplay();
        WriteString(10,  0, (uint8_t *)"Humidity");
        WriteString(10,  8, (uint8_t *)hum);
        WriteString(10, 16, (uint8_t *)"Pressure");
        WriteString(10, 24, (uint8_t *)pres);
        WriteString(10, 32, (uint8_t *)"Temperature");
        WriteString(10, 40, (uint8_t *)temp);
        UpdateDisplay();

        sleep_ms(1000);
    }
}