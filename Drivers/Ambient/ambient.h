#ifndef AMBIENT_H
#define AMBIENT_H

#include <stdbool.h>

#include "bme280_spi.h"
#include "ssd1306_font.h"
#include "ssd1306_i2c.h" 

#include <stdbool.h>
#include <stdint.h>
#include "bme280_spi.h"

static inline bool ambient_passes(void)
{
    uint8_t chip_id = 0;
    read_registers(0xD0, &chip_id, 1);
    if (chip_id != 0x60) {
        return false;
    }

    int32_t raw_humidity = 0;
    int32_t raw_pressure = 0;
    int32_t raw_temperature = 0;

    bme280_read_raw(&raw_humidity, &raw_pressure, &raw_temperature);

    // Temperature compensation sets t_fine for pressure and humidity.
    const double temp_c = compensate_temp(raw_temperature) / 100.0;
    const double pressure_kpa = compensate_pressure(raw_pressure) / 1000.0;
    const double humidity_pct = compensate_humidity(raw_humidity) / 1024.0;

    return temp_c >= 18.0 && temp_c <= 28.0 &&
           pressure_kpa >= 95.0 && pressure_kpa <= 105.0 &&
           humidity_pct >= 25.0 && humidity_pct <= 65.0;
}

#endif // AMBIENT_H