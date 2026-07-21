/**
 * @file driver_rtcc.h
 * @brief MCP79412 RTC control with 12/24H format handling.
 */
#ifndef DRIVER_RTCC_H
#define DRIVER_RTCC_H

#include <stdint.h>

typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t format_12h; // 1 = 12-Hour Format, 0 = 24-Hour Format
    uint8_t is_pm;      // 1 = PM, 0 = AM (Only used if format_12h == 1)
    uint8_t day;   
    uint8_t date;  
    uint8_t month; 
    uint8_t year;  
} RTC_TIME_t;

void RTC_Init(void);
void RTC_SetTime(RTC_TIME_t *time);
void RTC_GetTime(RTC_TIME_t *time);

#endif