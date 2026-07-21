/**
 * @file system.c
 * @brief Configuration bits and oscillator setup for PIC24FV32KA304.
 */

// FBS
#pragma config BWRP = OFF    //Boot Segment Write Protect->Disabled
#pragma config BSS = OFF    //Boot segment Protect->No boot program flash segment

// FGS
#pragma config GWRP = OFF    //General Segment Write Protect->General segment may be written
#pragma config GSS0 = OFF    //General Segment Code Protect->No Protection

// FOSCSEL
#pragma config FNOSC = FRCPLL    //Oscillator Select->Fast RC Oscillator with Postscaler and PLL Module (FRCDIV+PLL)
#pragma config SOSCSRC = DIG    //SOSC Source Type->Digital Mode for use with external source
#pragma config LPRCSEL = HP    //LPRC Oscillator Power and Accuracy->High Power, High Accuracy Mode
#pragma config IESO = ON    //Internal External Switch Over bit->Internal External Switchover mode enabled (Two-speed Start-up enabled)

// FOSC
#pragma config POSCMOD = NONE    //Primary Oscillator Configuration bits->Primary oscillator disabled
#pragma config OSCIOFNC = OFF    //CLKO Enable Configuration bit->CLKO output disabled
#pragma config POSCFREQ = HS    //Primary Oscillator Frequency Range Configuration bits->Primary oscillator/external clock input frequency greater than 8MHz
#pragma config SOSCSEL = SOSCHP    //SOSC Power Selection Configuration bits->Secondary Oscillator configured for high-power operation
#pragma config FCKSM = CSDCMD    //Clock Switching and Monitor Selection->Both Clock Switching and Fail-safe Clock Monitor are disabled

// FWDT
#pragma config WDTPS = PS32768    //Watchdog Timer Postscale Select bits->1:32768
#pragma config FWPSA = PR128    //WDT Prescaler bit->WDT prescaler ratio of 1:128
#pragma config FWDTEN = OFF    //Watchdog Timer Enable bits->WDT disabled in hardware; SWDTEN bit disabled
#pragma config WINDIS = OFF    //Windowed Watchdog Timer Disable bit->Standard WDT selected(windowed WDT disabled)

// FPOR
#pragma config BOREN = BOR3    //Brown-out Reset Enable bits->Brown-out Reset enabled in hardware, SBOREN bit disabled
#pragma config LVRCFG = OFF    //Low Voltage Regulator Configuration bit->Low Voltage regulator is not available
#pragma config PWRTEN = ON    //Power-up Timer Enable bit->PWRT enabled
#pragma config I2C1SEL = SEC    //Alternate I2C1 Pin Mapping bit->Use  Alternate SCL1/ASDA1 Pins For I2C1
#pragma config BORV = V20    //Brown-out Reset Voltage bits->Brown-out Reset set to lowest voltage (2.0V)
#pragma config MCLRE = ON    //MCLR Pin Enable bit->RA5 input pin disabled,MCLR pin enabled

// FICD
#pragma config ICS = PGx2    //ICD Pin Placement Select bits->EMUC/EMUD share PGC2/PGD2

// FDS
#pragma config DSWDTPS = DSWDTPSF    //Deep Sleep Watchdog Timer Postscale Select bits->1:2,147,483,648 (25.7 Days)
#pragma config DSWDTOSC = LPRC    //DSWDT Reference Clock Select bit->DSWDT uses Low Power RC Oscillator (LPRC)
#pragma config DSBOREN = ON    //Deep Sleep Zero-Power BOR Enable bit->Deep Sleep BOR enabled in Deep Sleep
#pragma config DSWDTEN = ON    //Deep Sleep Watchdog Timer Enable bit->DSWDT enabled

#include "system.h"

// --- PIC24FV32KA304 Configuration Bits ---
//#pragma config FNOSC = FRCPLL   // Fast RC Oscillator with PLL module (32 MHz system clock)
//#pragma config I2C1SEL = SEC    // Route I2C1 to Alternate Pins (ASCL1/ASDA1)
//#pragma config FWDTEN = OFF     // Disable Watchdog Timer for debugging
//#pragma config ICS = PGx2       // Emulator/debugger uses EMUC2/EMUD2

void SYSTEM_Initialize(void) {
    // Configure Oscillator for 32MHz (FCY = 16MHz)
    CLKDIVbits.RCDIV = 0; 
}
