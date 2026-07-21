/**
 * @file main.c
 * @brief Main execution entry point.
 */
#include "system.h"
#include "pin_manager.h"
#include "app_state_machine.h"

int main(void) {
    // 1. Core Hardware Init
    SYSTEM_Initialize();
    PIN_MANAGER_Initialize();
    
    // 2. Application/Driver Init
    App_Init();

    // 3. The Super-Loop
    while (1) {
        App_RunStateMachine();
        __delay_ms(10); // Standard base tick
    }
    
    return 1;
}