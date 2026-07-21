/**
 * @file hal_i2c.c
 * @brief I2C1 peripheral control. Operates at 100kHz on ASDA1/ASCL1.
 */
#include <xc.h>
#include "system.h"
#include "hal_i2c.h"

void HAL_I2C1_Init(void) {
    I2C1BRG = 39; // 100kHz at FCY 16MHz
    I2C1CONbits.I2CEN = 1;
}

static void I2C1_Wait(void) {
    while ((I2C1CON & 0x001F) || I2C1STATbits.TRSTAT);
}

// Used for raw devices like LCD
uint8_t HAL_I2C1_WriteBlock(uint8_t address, uint8_t *data, uint16_t length) {
    I2C1CONbits.SEN = 1; I2C1_Wait(); 
    
    I2C1TRN = (address << 1); I2C1_Wait(); 
    if(I2C1STATbits.ACKSTAT) return 0;     
    
    for(uint16_t i = 0; i < length; i++) {
        I2C1TRN = data[i]; I2C1_Wait();
    }
    
    I2C1CONbits.PEN = 1; I2C1_Wait(); 
    return 1;
}

// Used for memory-mapped devices like RTC/EEPROM
uint8_t HAL_I2C1_WriteRegisterBlock(uint8_t address, uint8_t reg, uint8_t *data, uint16_t length) {
    I2C1CONbits.SEN = 1; I2C1_Wait(); 
    
    I2C1TRN = (address << 1); I2C1_Wait(); 
    if(I2C1STATbits.ACKSTAT) return 0;  
    
    I2C1TRN = reg; I2C1_Wait(); 
    
    for(uint16_t i = 0; i < length; i++) {
        I2C1TRN = data[i]; I2C1_Wait();
    }
    
    I2C1CONbits.PEN = 1; I2C1_Wait(); 
    return 1;
}

uint8_t HAL_I2C1_ReadBlock(uint8_t address, uint8_t reg, uint8_t *data, uint16_t length) {
    I2C1CONbits.SEN = 1; I2C1_Wait(); 
    I2C1TRN = (address << 1); I2C1_Wait(); 
    I2C1TRN = reg; I2C1_Wait(); 
    
    I2C1CONbits.RSEN = 1; I2C1_Wait(); 
    I2C1TRN = (address << 1) | 1; I2C1_Wait(); 
    
    for(uint16_t i = 0; i < length; i++) {
        I2C1CONbits.RCEN = 1; I2C1_Wait();
        data[i] = I2C1RCV;
        I2C1CONbits.ACKDT = (i == length - 1) ? 1 : 0; 
        I2C1CONbits.ACKEN = 1; I2C1_Wait();
    }
    
    I2C1CONbits.PEN = 1; I2C1_Wait();
    return 1;
}