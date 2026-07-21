/**
 * @file driver_rtcc.c
 * @brief Interfaces with MCP79412 over I2C to parse BCD and format flags.
 */
#include <xc.h>
#include "driver_rtcc.h"
#include "hal_i2c.h"

#define RTC_ADDR 0x6F 

static uint8_t DecToBcd(uint8_t val) { return (uint8_t)(((val / 10) << 4) | (val % 10)); }
static uint8_t BcdToDec(uint8_t val) { return (uint8_t)(((val >> 4) * 10) + (val & 0x0F)); }

void RTC_Init(void) {
    HAL_I2C1_Init();
    uint8_t sec_reg = 0;
    HAL_I2C1_ReadBlock(RTC_ADDR, 0x00, &sec_reg, 1);
    
    // Safety Boot: If Oscillator Start (ST) bit is 0, inject default seed
    if ((sec_reg & 0x80) == 0x00) {
        RTC_TIME_t seed_time = {
            .sec = 0, .min = 0, .hour = 0, 
            .format_12h = 0, .is_pm = 0, 
            .day = 1, .date = 1, .month = 1, .year = 26
        };
        RTC_SetTime(&seed_time);
    }
}

void RTC_SetTime(RTC_TIME_t *t) {
    uint8_t data[7];
    data[0] = DecToBcd(t->sec) | 0x80;   
    data[1] = DecToBcd(t->min);
    
    // Parse 12/24 logic into MCP79412 hour register
    if (t->format_12h) {
        data[2] = DecToBcd(t->hour) | 0x40 | (t->is_pm ? 0x20 : 0x00); 
    } else {
        data[2] = DecToBcd(t->hour); 
    }
    
    data[3] = DecToBcd(t->day) | 0x08;   // Enable Battery Backup (VBATEN)
    data[4] = DecToBcd(t->date);
    data[5] = DecToBcd(t->month);
    data[6] = DecToBcd(t->year);
    
    HAL_I2C1_WriteRegisterBlock(RTC_ADDR, 0x00, data, 7);
}

void RTC_GetTime(RTC_TIME_t *t) {
    uint8_t data[7] = {0};
    HAL_I2C1_ReadBlock(RTC_ADDR, 0x00, data, 7);
    
    t->sec   = BcdToDec(data[0] & 0x7F);
    t->min   = BcdToDec(data[1] & 0x7F);
    
    // Extract format flags
    t->format_12h = (data[2] & 0x40) ? 1 : 0;
    if (t->format_12h) {
        t->is_pm = (data[2] & 0x20) ? 1 : 0;
        t->hour  = BcdToDec(data[2] & 0x1F);
    } else {
        t->is_pm = 0;
        t->hour  = BcdToDec(data[2] & 0x3F);
    }
    
    t->day   = BcdToDec(data[3] & 0x07);
    t->date  = BcdToDec(data[4] & 0x3F);
    t->month = BcdToDec(data[5] & 0x1F);
    t->year  = BcdToDec(data[6]);
}