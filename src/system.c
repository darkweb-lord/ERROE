/**
 * @file system.c
 * @brief Configuration bits and oscillator setup for PIC24FV32KA304.
 */
#include "system.h"

// --- PIC24FV32KA304 Configuration Bits ---
#pragma config FNOSC = FRCPLL   // Fast RC Oscillator with PLL module (32 MHz system clock)
#pragma config I2C1SEL = SEC    // Route I2C1 to Alternate Pins (ASCL1/ASDA1)
#pragma config FWDTEN = OFF     // Disable Watchdog Timer for debugging
#pragma config ICS = PGx2       // Emulator/debugger uses EMUC2/EMUD2

void SYSTEM_Initialize(void) {
    // Configure Oscillator for 32MHz (FCY = 16MHz)
    CLKDIVbits.RCDIV = 0; 
}
