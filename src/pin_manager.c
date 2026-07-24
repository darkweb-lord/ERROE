#include <xc.h>
#include <stdio.h>
#include "pin_manager.h"

/**
 Section: Driver Interface Function Definitions
*/
void PIN_MANAGER_Initialize (void)
{
    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATA = 0x0000;
    LATB = 0x0000;
    LATC = 0x0000;

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISA = 0x0F97;
    TRISB = 0xFFFF;
    TRISC = 0x03FF;

    /****************************************************************************
     * Setting the Weak Pull Up and Weak Pull Down SFR(s)
     ***************************************************************************/
    CNPD1 = 0x0000;
    CNPD2 = 0x0000;
    CNPD3 = 0x0000;
    CNPU1 = 0x0000;
    CNPU2 = 0x0000;
    CNPU3 = 0x0000;

    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCA = 0x0000;
    ODCB = 0x0060; //RB5 & RB6 as open Drain
    ODCC = 0x0000;

    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    ANSA = 0x000F;
    ANSB = 0xF01F;
    ANSC = 0x0007;
    
    // ==========================================
    // ATSENSE301 Metering IC Pin Configuration
    // ==========================================
    // Control Pins
    TRISBbits.TRISB4 = 0; // PIN 33 (RB4): meteringMclr - Output
    TRISAbits.TRISA4 = 1; // PIN 34 (RA4): metering_interrupt - Input
    
    // SPI1 Data/Clock Pins
    TRISAbits.TRISA9 = 0; // PIN 35 (RA9): chip select - Output
    TRISCbits.TRISC3 = 1; // PIN 36 (RC3): MISO (SDI1) - Input
    TRISCbits.TRISC4 = 0; // PIN 37 (RC4): MOSI (SDO1) - Output
    TRISCbits.TRISC5 = 0; // PIN 38 (RC5): Clock (SCK1) - Output

    // ==========================================
    // Analog Function Disablement (CRITICAL)
    // ==========================================
    ANSBbits.ANSB13 = 0;  // Force RB13 (Button) to Digital Mode
    ANSBbits.ANSB4 = 0;   // Force RB4 (meteringMclr) to Digital Mode
    
    // Set default initial states for active-low outputs
    LATAbits.LATA9 = 1;   // CS High (Deselected)
    LATBbits.LATB4 = 1;   // MCLR High (Not in reset)
    
}

