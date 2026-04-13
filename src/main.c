#include "system.h"
#include<xc.h>
#include "I2c_Header.h"
#include "LCD_I2C.h"


/*
                        Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    I2C_INIT();
    LCD_INIT();
   
    while (1)
    {
        LCD_SetCursor(0,0);
        LCD_PRINT("System Booting...");
        LCD_SetCursor(1,0);
        LCD_PRINT("Made by DISPL");
    }

    return 1;
}
/**
 End of File
*/