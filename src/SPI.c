#include "SPI.h"

// Define FCY for __delay_ms() - Ensure this matches your system clock
#define FCY 16000000UL 
#include <libpic30.h>  

void SPI1_Initialize(void) {
    // 1. Disable SPI module during configuration
    SPI1STATbits.SPIEN = 0; 
    
    // 2. Clear SPI interrupt flags
    IFS0bits.SPI1IF = 0;    
    IEC0bits.SPI1IE = 0;    

    // 3. SPI1CON1 Configuration
    // MSTEN = 1, CKP = 0, CKE = 1, SMP = 0 (Standard SPI Mode 1)
    SPI1CON1 = 0x013A; 
    
    // 4. SPI1CON2 Configuration
    SPI1CON2 = 0x0000;

    // 5. Ensure Chip Select is High (Deselected) before enabling SPI
    ATSENSE_CS_LAT = 1;  

    // 6. Enable SPI Module
    SPI1STATbits.SPIEN = 1; 
}

void ATSENSE_HardwareReset(void) {
    // Toggle the MCLR pin to hard-reset the metering IC
    ATSENSE_MCLR_LAT = 0; // Pull MCLR low
    __delay_ms(10);       // Hold for 10ms
    ATSENSE_MCLR_LAT = 1; // Release MCLR high
    __delay_ms(50);       // Wait 50ms for ATSENSE boot sequence to complete
}

uint8_t SPI1_Exchange8bit(uint8_t data) {
    SPI1STATbits.SPIRBF = 0; // Clear the receive buffer full flag
    SPI1BUF = data;          // Write data to transmit buffer
    while (!SPI1STATbits.SPIRBF); // Wait for transmission to complete
    return SPI1BUF;          // Return the received data
}

void ATSENSE_WriteRegister(uint8_t reg_address, uint32_t data) {
    ATSENSE_CS_LAT = 0; // Assert CS (Low)
    
    // Transmit Command (Write bit = 1) + 7-bit Address
    SPI1_Exchange8bit(0x80 | (reg_address & 0x7F)); 
    
    // Transmit 32-bit Data (MSB first)
    SPI1_Exchange8bit((data >> 24) & 0xFF);
    SPI1_Exchange8bit((data >> 16) & 0xFF);
    SPI1_Exchange8bit((data >> 8) & 0xFF);
    SPI1_Exchange8bit(data & 0xFF);
    
    ATSENSE_CS_LAT = 1; // De-assert CS (High)
}

uint32_t ATSENSE_ReadRegister(uint8_t reg_address) {
    uint32_t read_value = 0;
    
    ATSENSE_CS_LAT = 0; // Assert CS (Low)
    
    // Transmit Command (Write bit = 0) + 7-bit Address
    SPI1_Exchange8bit(reg_address & 0x7F);
    
    // Receive 32-bit Data by clocking out dummy bytes (0xFF)
    read_value |= ((uint32_t)SPI1_Exchange8bit(0xFF) << 24);
    read_value |= ((uint32_t)SPI1_Exchange8bit(0xFF) << 16);
    read_value |= ((uint32_t)SPI1_Exchange8bit(0xFF) << 8);
    read_value |= ((uint32_t)SPI1_Exchange8bit(0xFF));
    
    ATSENSE_CS_LAT = 1; // De-assert CS (High)
    
    return read_value;
}