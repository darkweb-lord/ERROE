/**
 * @file hal_i2c.c
 * @brief I2C1 peripheral control. Operates at 100kHz on ASDA1/ASCL1.
 */
#include <xc.h>
#include "system.h"
#include "hal_i2c.h"

// =========================================================================
// BUS RECOVERY PINS
// ACTION REQUIRED: Update these macros to match your exact ASCL1/ASDA1 pins
// =========================================================================
#define I2C_SCL_LAT   LATBbits.LATB5   // <-- Replace LATB5 with your ASCL1 LAT
#define I2C_SCL_TRIS  TRISBbits.TRISB5 // <-- Replace TRISB5 with your ASCL1 TRIS
#define I2C_SDA_LAT   LATBbits.LATB6   // <-- Replace LATB6 with your ASDA1 LAT
#define I2C_SDA_PORT  PORTBbits.RB6    // <-- Replace RB6 with your ASDA1 PORT
#define I2C_SDA_TRIS  TRISBbits.TRISB6 // <-- Replace TRISB6 with your ASDA1 TRIS
// =========================================================================

void HAL_I2C1_BusRecovery(void) {
    // 1. Disable hardware I2C module to take manual GPIO control
    I2C1CONbits.I2CEN = 0;
    
    // 2. Set Latches to 0. We will simulate Open-Drain by toggling TRIS.
    // TRIS = 1 (Input) -> Pulled HIGH by external pull-up resistors
    // TRIS = 0 (Output) -> Drives LOW because LAT is 0
    I2C_SCL_LAT = 0;
    I2C_SDA_LAT = 0;
    
    I2C_SDA_TRIS = 1; // Release SDA
    I2C_SCL_TRIS = 1; // Release SCL
    __delay_us(10);
    
    // 3. Send up to 9 clock pulses to clear any stuck slave state machines
    for (uint8_t i = 0; i < 9; i++) {
        if (I2C_SDA_PORT == 1) {
            break; // Slave released SDA, bus is free!
        }
        I2C_SCL_TRIS = 0; // Drive SCL Low
        __delay_us(10);
        I2C_SCL_TRIS = 1; // Release SCL High
        __delay_us(10);
    }
    
    // 4. Generate Manual STOP Condition (SDA goes Low -> High while SCL is High)
    I2C_SDA_TRIS = 0; // Drive SDA Low
    __delay_us(10);
    I2C_SCL_TRIS = 1; // Ensure SCL is High
    __delay_us(10);
    I2C_SDA_TRIS = 1; // Release SDA High (Creates the STOP)
    __delay_us(10);
}

void HAL_I2C1_Init(void) {
    // Run Bus Recovery before turning on the I2C hardware
    HAL_I2C1_BusRecovery(); 
    
    I2C1BRG = 39; // 100kHz at FCY 16MHz
    I2C1CONbits.I2CEN = 1; // Enable hardware I2C module
}

static void I2C1_Wait(void) {
    while ((I2C1CON & 0x001F) || I2C1STATbits.TRSTAT);
}

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