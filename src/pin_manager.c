/**
 * @file pin_manager.c
 * @brief Sets initial pin states and analog/digital modes.
 */
#include <xc.h>
#include "pin_manager.h"

void PIN_MANAGER_Initialize (void) {
    // 1. Clear Latches to prevent voltage spikes
    LATA = 0x0000;
    LATB = 0x0000;
    LATC = 0x0000;

    // 2. Setting the PIN Direction SFR(s)
    TRISCbits.TRISC2 = 1; // KEY_1
    TRISAbits.TRISA8 = 1; // KEY_2
    
    TRISCbits.TRISC8 = 1; // Tamper 
    
    // 3. Disable Analog on digital pins
    ANSAbits.ANSA1 = 0;  // Spare 1 LED
    ANSBbits.ANSB0 = 0;  // spare 2 LED
    ANSBbits.ANSB1 = 0;  // spare 3 LED
    ANSBbits.ANSB12 = 0; // RTCC MFP - Multi Function PIN
    ANSBbits.ANSB2 = 0;  // R-Phase Relay
    ANSBbits.ANSB3 = 0;  // Y-Phase Relay
    ANSBbits.ANSB4 = 0;  // Metering_Master_Clear
    ANSCbits.ANSC0 = 0;  // B-Phase Relay
    ANSCbits.ANSC1 = 0;  // N-Neutral Relay
    ANSCbits.ANSC2 = 0;  // KEY_1
}