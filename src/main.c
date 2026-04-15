#include "system.h"
#include<xc.h>
#include "I2c_Header.h"
#include "LCD_I2C.h"
#include "RTCC.h"
#include <libpic30.h>
#include<stdio.h>

/*
Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    I2C_INIT();
    LCD_INIT();
    RTC_Init();
    
    RTC_TIME_t myTime;
    
    char lcdBuffer[17];
   
    while (1)
    {
        LCD_SetCursor(0,0);
        LCD_PRINT("System Booting...");
        LCD_SetCursor(1,0);
        LCD_PRINT("Made by DISPL");
        __delay_ms(1000);
        
        // Ask the RTC for the current time
        RTC_GetTime(&myTime);
        
        // Format the time into a string (HH:MM:SS)
        sprintf(lcdBuffer, "Time: %02d:%02d:%02d", myTime.hour, myTime.min, myTime.sec);
        LCD_SetCursor(0,0);
        LCD_PRINT(lcdBuffer);
        
        // Format the date into a string (DD/MM/YY)
        sprintf(lcdBuffer, "Date: %02d/%02d/20%02d", myTime.date, myTime.month, myTime.year);
        LCD_SetCursor(1,0);
        LCD_PRINT(lcdBuffer);
        
        // Use a non-blocking timer in the future, but a small delay is fine for testing
        // __delay_ms(200); 
    }

    return 1;
}
/**
 End of File
*/
