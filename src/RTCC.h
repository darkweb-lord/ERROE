#ifndef RTCC_H
#define RTCC_H

#include <stdint.h>

// Struct to hold our standard decimal time variables
typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;   // 0-6 (Weekday)
    uint8_t date;  // 1-31 (Day of Month)
    uint8_t month; // 1-12
    uint8_t year;  // 0-99
} RTC_TIME_t;

void RTC_Init(void);
void RTC_SetTime(RTC_TIME_t *time);
void RTC_GetTime(RTC_TIME_t *time);

#endif /* RTCC_H */