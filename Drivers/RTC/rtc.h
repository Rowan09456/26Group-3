#ifndef RTC_H
#define RTC_H

#include <stdbool.h>
#include <stdint.h>
#include "hardware/i2c.h"

#define DS3231_ADDR 0x68

typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day_of_week;
    uint8_t date;
    uint8_t month;
    uint8_t year;
} ds3231_dateTime;

static i2c_inst_t *rtc_port = NULL;

static inline uint8_t rtc_to_bcd(uint8_t value)
{
    return (uint8_t)(((value / 10) << 4) | (value % 10));
}

static inline uint8_t rtc_from_bcd(uint8_t value)
{
    return (uint8_t)(((value >> 4) * 10) + (value & 0x0F));
}

static inline void ds3231_init(i2c_inst_t *port)
{
    rtc_port = port;
}

static inline bool ds3231_set_time(ds3231_dateTime dt)
{
    if (rtc_port == NULL) {
        return false;
    }

    uint8_t buf[8] = {
        0x00,
        rtc_to_bcd(dt.sec),
        rtc_to_bcd(dt.min),
        rtc_to_bcd(dt.hour),  // Write in 24-hour mode.
        rtc_to_bcd(dt.day_of_week),
        rtc_to_bcd(dt.date),
        rtc_to_bcd(dt.month),
        rtc_to_bcd(dt.year)
    };

    return i2c_write_blocking(
        rtc_port, DS3231_ADDR, buf, sizeof(buf), false
    ) == (int)sizeof(buf);
}

static inline bool ds3231_read_time(ds3231_dateTime *dt)
{
    if (rtc_port == NULL || dt == NULL) {
        return false;
    }

    uint8_t reg = 0x00;
    uint8_t buf[7];

    if (i2c_write_blocking(
            rtc_port, DS3231_ADDR, &reg, 1, true) != 1) {
        return false;
    }

    if (i2c_read_blocking(
            rtc_port, DS3231_ADDR, buf, sizeof(buf), false)
            != (int)sizeof(buf)) {
        return false;
    }

    dt->sec = rtc_from_bcd(buf[0] & 0x7F);
    dt->min = rtc_from_bcd(buf[1] & 0x7F);

    if (buf[2] & 0x40) {
        // Convert 12-hour mode to 24-hour time.
        dt->hour = rtc_from_bcd(buf[2] & 0x1F) % 12;
        if (buf[2] & 0x20) {
            dt->hour += 12;
        }
    } else {
        dt->hour = rtc_from_bcd(buf[2] & 0x3F);
    }

    dt->day_of_week = rtc_from_bcd(buf[3] & 0x07);
    dt->date = rtc_from_bcd(buf[4] & 0x3F);
    dt->month = rtc_from_bcd(buf[5] & 0x1F);
    dt->year = rtc_from_bcd(buf[6]);

    return true;
}

#endif