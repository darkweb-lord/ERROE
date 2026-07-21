/**
 * @file driver_button.c
 * @brief Non-blocking debouncer for UI input.
 */
#include <xc.h>
#include "driver_button.h"

#define K1_PIN PORTCbits.RC2
#define K2_PIN PORTAbits.RA8
#define BTN_PRESSED 0

ButtonEvent_t Button_Read_Matrix(void) {
    static uint16_t both_timer = 0, k1_timer = 0, k2_timer = 0;
    static uint8_t long_triggered = 0;
    ButtonEvent_t event = EVENT_NONE;

    uint8_t b1 = (K1_PIN == BTN_PRESSED);
    uint8_t b2 = (K2_PIN == BTN_PRESSED);

    if (b1 && b2) {
        both_timer++; k1_timer = 0; k2_timer = 0;
        if (both_timer > 100 && !long_triggered) { 
            event = EVENT_BOTH_LONG; long_triggered = 1;
        }
    } else if (b1) { 
        k1_timer++; both_timer = 0; long_triggered = 0; 
    } else if (b2) { 
        k2_timer++; both_timer = 0; long_triggered = 0; 
    } else {
        if (k1_timer > 5 && !long_triggered) event = EVENT_K1_SHORT;
        if (k2_timer > 5 && !long_triggered) event = EVENT_K2_SHORT;
        both_timer = 0; k1_timer = 0; k2_timer = 0; long_triggered = 0;
    }
    return event;
}
