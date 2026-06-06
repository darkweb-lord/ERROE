#include <xc.h>
#include "RTCC.h"

// Helper Math Functions for BCD conversion
uint8_t DecToBcd(uint8_t val) {
    return (uint8_t)(((val / 10) << 4) | (val % 10));
}

uint8_t BcdToDec(uint8_t val) {
    return (uint8_t)(((val >> 4) * 10) + (val & 0x0F));
}

void RTC_Init(void) {
    // 1. Enable Secondary Oscillator (SOSC) to drive the RTCC
    __builtin_write_OSCCONL(OSCCON | 0x02); 

    // 2. Unlock the RTCC registers and Enable it
    __builtin_write_RTCWEN(); 
    RCFGCALbits.RTCEN = 1; 
    RCFGCALbits.RTCWREN = 0; 

    // 3. Safety Check: If the Month is '0', the clock will NEVER tick.
    // We must read the month/date register to check.
    RCFGCALbits.RTCPTR = 2; 
    uint16_t current_month_date = RTCVAL;
    
    if (((current_month_date >> 8) & 0xFF) == 0x00) {
        // If blank, inject a valid seed date: Jan 1, 2026, 00:00:00
        RTC_TIME_t seed_time = {0, 0, 0, 1, 1, 1, 26};
        RTC_SetTime(&seed_time);
    }
}

void RTC_SetTime(RTC_TIME_t *t) {
    __builtin_write_RTCWEN(); // Unlock sequence
    
    // Disable RTCC temporarily to write new values safely
    RCFGCALbits.RTCEN = 0;    
    
    // Set pointer to Year (3). 
    // It will AUTO-DECREMENT after every write to RTCVAL.
    RCFGCALbits.RTCPTR = 3; 
    
    RTCVAL = DecToBcd(t->year);                               // Pointer drops to 2
    RTCVAL = (DecToBcd(t->month) << 8) | DecToBcd(t->date);   // Pointer drops to 1
    RTCVAL = (DecToBcd(t->day) << 8)   | DecToBcd(t->hour);   // Pointer drops to 0
    RTCVAL = (DecToBcd(t->min) << 8)   | DecToBcd(t->sec);    // Finished
    
    // Re-enable and Lock
    RCFGCALbits.RTCEN = 1;    
    RCFGCALbits.RTCWREN = 0;  
}

void RTC_GetTime(RTC_TIME_t *t) {
    // Wait until the RTCC is not actively rolling over before reading
    while(RCFGCALbits.RTCSYNC == 1);
    
    // Reading RTCVAL automatically decrements the internal pointer
    RCFGCALbits.RTCPTR = 3; 
    
    uint16_t year_reg       = RTCVAL; // Pointer drops to 2
    uint16_t month_date_reg = RTCVAL; // Pointer drops to 1
    uint16_t wday_hour_reg  = RTCVAL; // Pointer drops to 0
    uint16_t min_sec_reg    = RTCVAL; 
    
    t->year  = BcdToDec(year_reg & 0xFF);
    t->month = BcdToDec((month_date_reg >> 8) & 0xFF);
    t->date  = BcdToDec(month_date_reg & 0xFF);
    t->day   = BcdToDec((wday_hour_reg >> 8) & 0xFF);
    t->hour  = BcdToDec(wday_hour_reg & 0xFF);
    t->min   = BcdToDec((min_sec_reg >> 8) & 0xFF);
    t->sec   = BcdToDec(min_sec_reg & 0xFF);
}