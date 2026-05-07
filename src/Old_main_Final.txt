#include "system.h"
#include <xc.h>

// 1. MUST define FCY before libpic30.h for delays to work
#define FCY 4000000UL 
#include <libpic30.h>
#include <stdio.h>
#include <string.h>

#include "I2c_Header.h"
#include "LCD_I2C.h"
#include "RTCC.h"

// --- Hardware Definitions ---
#define KEY_1 PORTAbits.RA8   // Scroll Down / Increment / Toggle
#define KEY_2 PORTCbits.RC2   // Scroll Up / Decrement / Toggle
#define BTN_PRESSED 0         // Assuming buttons pull to Ground

// --- System States ---
typedef enum {
    STATE_BOOT,
    STATE_HOME,
    STATE_MENU,
    STATE_PAGE_0,
    STATE_PAGE_1,
    STATE_PAGE_2,
    STATE_PAGE_TIME,
    STATE_EDIT_RTC,
    STATE_SAVE_PROMPT   // New State for Save confirmation
} SystemState_t;

// --- Button Event Types ---
typedef enum { 
    EVENT_NONE, 
    EVENT_K1_SHORT, 
    EVENT_K2_SHORT, 
    EVENT_BOTH_LONG 
} ButtonEvent_t;

// --- Global Variables ---
SystemState_t system_state = STATE_BOOT;
const char* menu_items[4] = {"1.Page_0", "2.Page_1", "3.Page_2", "4.Time/Date"};
int8_t menu_index = 0;
uint8_t update_screen = 1;
RTC_TIME_t rtc_time;

// Edit Mode Variables
RTC_TIME_t edit_time;
uint8_t edit_field = 0; // 0=Hr, 1=Min, 2=Sec, 3=Date, 4=Month, 5=Year
uint8_t save_choice = 1; // 1 = YES, 0 = NO
uint8_t blink_state = 1; // Used to toggle text for blinking effect

// --- Button Debounce Function ---
ButtonEvent_t Read_Buttons(void) {
    static uint16_t both_timer = 0;
    static uint16_t k1_timer = 0;
    static uint16_t k2_timer = 0;
    static uint8_t long_triggered = 0;
    
    ButtonEvent_t event = EVENT_NONE;
    uint8_t b1 = (KEY_1 == BTN_PRESSED);
    uint8_t b2 = (KEY_2 == BTN_PRESSED);

    if (b1 && b2) {
        both_timer++;
        k1_timer = 0; k2_timer = 0;
        if (both_timer > 100 && !long_triggered) { // 100 * 10ms = 1 Second
            event = EVENT_BOTH_LONG;
            long_triggered = 1;
        }
    } else if (b1) {
        k1_timer++;
        both_timer = 0; long_triggered = 0;
    } else if (b2) {
        k2_timer++;
        both_timer = 0; long_triggered = 0;
    } else {
        // Buttons Released
        if (k1_timer > 5 && !long_triggered) event = EVENT_K1_SHORT;
        if (k2_timer > 5 && !long_triggered) event = EVENT_K2_SHORT;
        
        both_timer = 0; k1_timer = 0; k2_timer = 0; long_triggered = 0;
    }
    return event;
}

// --- Main Application ---
int main(void) {
    SYSTEM_Initialize();
    I2C_INIT();
    LCD_INIT();
    RTC_Init();
    
    // Configure Pins for Buttons
    TRISAbits.TRISA8 = 1; // Key 1 Input
    TRISCbits.TRISC2 = 1; // Key 2 Input
    ANSCbits.ANSC2 = 0;   // Disable Analog on RC2
    
    char lcdBuffer[17];
    uint16_t clock_tick = 0;
    uint16_t blink_timer = 0;

    while (1) {
        // 1. Read Button Events
        ButtonEvent_t btn_event = Read_Buttons();
        
        // 2. Handle State Transitions based on Buttons
        if (btn_event != EVENT_NONE) {
            update_screen = 1; // Force screen update on any button press
            blink_state = 1;   // Force text to show immediately on press
            blink_timer = 0;
            
            switch(system_state) {
                case STATE_HOME:
                    if (btn_event == EVENT_BOTH_LONG) system_state = STATE_MENU;
                    break;
                    
                case STATE_MENU:
                    if (btn_event == EVENT_K1_SHORT) { // Scroll Down
                        menu_index++;
                        if (menu_index > 3) menu_index = 0;
                    }
                    if (btn_event == EVENT_K2_SHORT) { // Scroll Up
                        menu_index--;
                        if (menu_index < 0) menu_index = 3;
                    }
                    if (btn_event == EVENT_BOTH_LONG) { // Enter Selected Page
                        if (menu_index == 0) system_state = STATE_PAGE_0;
                        if (menu_index == 1) system_state = STATE_PAGE_1;
                        if (menu_index == 2) system_state = STATE_PAGE_2;
                        if (menu_index == 3) system_state = STATE_PAGE_TIME;
                    }
                    break;
                    
                case STATE_PAGE_0:
                case STATE_PAGE_1:
                case STATE_PAGE_2:
                    if (btn_event == EVENT_BOTH_LONG) system_state = STATE_MENU; // Go back
                    break;
                    
                case STATE_PAGE_TIME:
                    if (btn_event == EVENT_BOTH_LONG) {
                        // Enter Edit Mode: Load current time into the editor
                        RTC_GetTime(&edit_time);
                        edit_field = 0; // Start at Hour
                        system_state = STATE_EDIT_RTC;
                    }
                    if (btn_event == EVENT_K1_SHORT || btn_event == EVENT_K2_SHORT) {
                        system_state = STATE_MENU; // Go back to menu on short press
                    }
                    break;
                    
                case STATE_EDIT_RTC:
                    if (btn_event == EVENT_BOTH_LONG) { // Next Field
                        edit_field++;
                        if (edit_field > 5) {
                            save_choice = 1; // Default to YES
                            system_state = STATE_SAVE_PROMPT; // Go to save screen
                        }
                    }
                    else if (btn_event == EVENT_K1_SHORT) { // INCREMENT (+)
                        if(edit_field == 0) { edit_time.hour++; if(edit_time.hour > 23) edit_time.hour = 0; }
                        if(edit_field == 1) { edit_time.min++;  if(edit_time.min > 59) edit_time.min = 0; }
                        if(edit_field == 2) { edit_time.sec++;  if(edit_time.sec > 59) edit_time.sec = 0; } 
                        if(edit_field == 3) { edit_time.date++; if(edit_time.date > 31) edit_time.date = 1; }
                        if(edit_field == 4) { edit_time.month++;if(edit_time.month > 12) edit_time.month = 1; }
                        if(edit_field == 5) { edit_time.year++; if(edit_time.year > 99) edit_time.year = 0; }
                    }
                    else if (btn_event == EVENT_K2_SHORT) { // DECREMENT (-)
                        if(edit_field == 0) { if(edit_time.hour == 0) edit_time.hour = 23; else edit_time.hour--; }
                        if(edit_field == 1) { if(edit_time.min == 0) edit_time.min = 59; else edit_time.min--; }
                        if(edit_field == 2) { if(edit_time.sec == 0) edit_time.sec = 59; else edit_time.sec--; }
                        if(edit_field == 3) { if(edit_time.date <= 1) edit_time.date = 31; else edit_time.date--; }
                        if(edit_field == 4) { if(edit_time.month <= 1) edit_time.month = 12; else edit_time.month--; }
                        if(edit_field == 5) { if(edit_time.year == 0) edit_time.year = 99; else edit_time.year--; }
                    }
                    break;
                    
                case STATE_SAVE_PROMPT:
                    if (btn_event == EVENT_K1_SHORT || btn_event == EVENT_K2_SHORT) {
                        save_choice = !save_choice; // Toggle between YES and NO
                    }
                    if (btn_event == EVENT_BOTH_LONG) {
                        if (save_choice == 1) {
                            RTC_SetTime(&edit_time); // Write new time to MCP79412
                        }
                        system_state = STATE_PAGE_TIME; // Return to view mode
                    }
                    break;
            }
        }
        
        // 3. Timers for Clock Updates and UI Blinking
        if (system_state == STATE_HOME || system_state == STATE_PAGE_TIME) {
            clock_tick++;
            if (clock_tick >= 100) { // 100 * 10ms = 1 sec
                clock_tick = 0;
                update_screen = 1;
            }
        }
        
        if (system_state == STATE_EDIT_RTC) {
            blink_timer++;
            if (blink_timer >= 50) { // 500ms blink rate
                blink_timer = 0;
                blink_state = !blink_state;
                update_screen = 1;
            }
        }
        
        // 4. Update LCD Screen
        if (update_screen) {
            update_screen = 0;
            
            switch(system_state) {
                case STATE_BOOT:
                    LCD_SetCursor(0,0); LCD_PRINT("System Booting..");
                    LCD_SetCursor(1,0); LCD_PRINT("Made by DISPL   ");
                    __delay_ms(2000); 
                    system_state = STATE_HOME;
                    update_screen = 1;
                    break;
                    
                case STATE_HOME:
                    RTC_GetTime(&rtc_time);
                    sprintf(lcdBuffer, "Time: %02d:%02d:%02d", rtc_time.hour, rtc_time.min, rtc_time.sec);
                    LCD_SetCursor(0,0); LCD_PRINT(lcdBuffer);
                    LCD_SetCursor(1,0); LCD_PRINT("Hold BOTH ->Menu");
                    break;
                    
                case STATE_MENU:
                    {
                        int top_line = (menu_index == 3) ? 2 : menu_index; 
                        
                        sprintf(lcdBuffer, "%c%s             ", (menu_index == top_line) ? '>' : ' ', menu_items[top_line]);
                        LCD_SetCursor(0,0); LCD_PRINT(lcdBuffer);
                        
                        sprintf(lcdBuffer, "%c%s             ", (menu_index == top_line+1) ? '>' : ' ', menu_items[top_line+1]);
                        LCD_SetCursor(1,0); LCD_PRINT(lcdBuffer);
                    }
                    break;
                    
                case STATE_PAGE_0:
                case STATE_PAGE_1:
                case STATE_PAGE_2:
                    sprintf(lcdBuffer, "Inside Page_%d   ", menu_index);
                    LCD_SetCursor(0,0); LCD_PRINT(lcdBuffer);
                    LCD_SetCursor(1,0); LCD_PRINT("Hold BOTH-> Back");
                    break;
                    
                case STATE_PAGE_TIME:
                    RTC_GetTime(&rtc_time);
                    sprintf(lcdBuffer, "Time: %02d:%02d:%02d", rtc_time.hour, rtc_time.min, rtc_time.sec);
                    LCD_SetCursor(0,0); LCD_PRINT(lcdBuffer);
                    sprintf(lcdBuffer, "Date: %02d/%02d/%02d", rtc_time.date, rtc_time.month, rtc_time.year);
                    LCD_SetCursor(1,0); LCD_PRINT(lcdBuffer);
                    break;
                    
                case STATE_EDIT_RTC:
                    {
                        char hr[3], mn[3], sc[3], dt[3], mo[3], yr[3];
                        
                        // If it's the active field AND blink state is off, show spaces. Otherwise, show the number.
                        if(edit_field == 0 && !blink_state) strcpy(hr, "  "); else sprintf(hr, "%02d", edit_time.hour);
                        if(edit_field == 1 && !blink_state) strcpy(mn, "  "); else sprintf(mn, "%02d", edit_time.min);
                        if(edit_field == 2 && !blink_state) strcpy(sc, "  "); else sprintf(sc, "%02d", edit_time.sec);
                        if(edit_field == 3 && !blink_state) strcpy(dt, "  "); else sprintf(dt, "%02d", edit_time.date);
                        if(edit_field == 4 && !blink_state) strcpy(mo, "  "); else sprintf(mo, "%02d", edit_time.month);
                        if(edit_field == 5 && !blink_state) strcpy(yr, "  "); else sprintf(yr, "%02d", edit_time.year);

                        sprintf(lcdBuffer, "Time: %s:%s:%s", hr, mn, sc);
                        LCD_SetCursor(0,0); LCD_PRINT(lcdBuffer);
                        
                        sprintf(lcdBuffer, "Date: %s/%s/%s", dt, mo, yr);
                        LCD_SetCursor(1,0); LCD_PRINT(lcdBuffer);
                    }
                    break;
                    
                case STATE_SAVE_PROMPT:
                    LCD_SetCursor(0,0); LCD_PRINT("Save Changes?   ");
                    if (save_choice == 1) {
                        LCD_SetCursor(1,0); LCD_PRINT(">YES     NO     ");
                    } else {
                        LCD_SetCursor(1,0); LCD_PRINT(" YES    >NO     ");
                    }
                    break;
            }
        }
        
        // 5. Global Super-Loop Tick (10ms)
        __delay_ms(10);
    }
    return 1;
}