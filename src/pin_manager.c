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

    // 2. Set Button Pins as Inputs (Explorer 16 Switches S4 & S5)
    TRISAbits.TRISA8 = 1; // KEY_1
    TRISBbits.TRISB3 = 1; // KEY_2

    // 3. Disable Analog on digital pins
    ANSBbits.ANSB3 = 0;   // Required to read the digital switch on RB3
}