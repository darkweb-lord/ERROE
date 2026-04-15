//Real-Time Clock and Calendar.- RTCC
//IC - MCP79412


#ifndef MCP79412_H
#define MCP79412_H

#include <stdint.h>

// Struct to hold our standard decimal time variables
typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;   // 1-7
    uint8_t date;  // 1-31
    uint8_t month; // 1-12
    uint8_t year;  // 0-99
} RTC_TIME_t;

void RTC_Init(void);
void RTC_SetTime(RTC_TIME_t *time);
void RTC_GetTime(RTC_TIME_t *time);

#endif /* MCP79412_H */