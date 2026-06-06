#include "system.h"
#include <xc.h>

// 1. MUST define FCY before libpic30.h for delays to work
#define FCY 16000000UL 
#include <libpic30.h>
#include <stdio.h>
#include <string.h>

#include "LCD.h"
#include "RTCC.h"

// --- Hardware Definitions ---
#define KEY_1 PORTAbits.RA8   // Button S4
#define KEY_2 PORTBbits.RB3   // Button S5
#define BTN_PRESSED 0         

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
    STATE_SAVE_PROMPT   
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

// Menu setup including Exit option
#define NUM_MENU_ITEMS 5
const char* menu_items[NUM_MENU_ITEMS] = {"1.Page_0", "2.Page_1", "3.Page_2", "4.Time/Date", "5.Exit Menu"};
int8_t menu_index = 0;

uint8_t update_screen = 1;
RTC_TIME_t rtc_time;
uint8_t last_second = 60; 

// Edit Mode Variables
RTC_TIME_t edit_time;
uint8_t edit_field = 0; 
uint8_t save_choice = 1; 
uint8_t blink_state = 1; 

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
        if (both_timer > 100 && !long_triggered) { 
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
        if (k1_timer > 5 && !long_triggered) event = EVENT_K1_SHORT;
        if (k2_timer > 5 && !long_triggered) event = EVENT_K2_SHORT;
        
        both_timer = 0; k1_timer = 0; k2_timer = 0; long_triggered = 0;
    }
    return event;
}

// --- Main Application ---
int main(void) {
    SYSTEM_Initialize();
    
    LCD_INIT();
    RTC_Init();
    
    // Configure Pins for Buttons 
    TRISAbits.TRISA8 = 1; 
    TRISBbits.TRISB3 = 1; 
    ANSBbits.ANSB3 = 0;   

    char lcdBuffer[17];
    uint16_t clock_tick = 0;
    uint16_t blink_timer = 0;
    
    // ==========================================
    // SCROLLING TEXT VARIABLES
    // ==========================================
    uint16_t scroll_tick = 0; // Tracks the 10ms loop to control speed
    uint16_t scroll_pos = 0;  // Tracks which letter is currently first on the LCD

    while (1) {
        // 1. Read Button Events
        ButtonEvent_t btn_event = Read_Buttons();
        
        // 2. Handle State Transitions
        if (btn_event != EVENT_NONE) {
            update_screen = 1; 
            blink_state = 1;   
            blink_timer = 0;
            
            switch(system_state){
                case STATE_HOME:
                    if (btn_event == EVENT_BOTH_LONG) system_state = STATE_MENU;
                    break;
                    
                case STATE_MENU:
                    if (btn_event == EVENT_K1_SHORT) { 
                        menu_index++;
                        if (menu_index >= NUM_MENU_ITEMS) menu_index = 0;
                    }
                    if (btn_event == EVENT_K2_SHORT) { 
                        menu_index--;
                        if (menu_index < 0) menu_index = NUM_MENU_ITEMS - 1;
                    }
                    if (btn_event == EVENT_BOTH_LONG) { 
                        if (menu_index == 0) system_state = STATE_PAGE_0;
                        if (menu_index == 1) system_state = STATE_PAGE_1;
                        if (menu_index == 2) system_state = STATE_PAGE_2;
                        if (menu_index == 3) system_state = STATE_PAGE_TIME;
                        if (menu_index == 4) {
                            system_state = STATE_HOME;
                            menu_index = 0; 
                            LCD_CLEAR();    
                        }
                    }
                    break;
                    
                case STATE_PAGE_0:
                case STATE_PAGE_1:
                case STATE_PAGE_2:
                    if (btn_event == EVENT_BOTH_LONG) system_state = STATE_MENU;
                    break;
                    
                case STATE_PAGE_TIME:
                    if (btn_event == EVENT_BOTH_LONG) {
                        RTC_GetTime(&edit_time);
                        edit_field = 0; 
                        system_state = STATE_EDIT_RTC;
                    }
                    if (btn_event == EVENT_K1_SHORT || btn_event == EVENT_K2_SHORT) {
                        system_state = STATE_MENU; 
                    }
                    break;
                    
                case STATE_EDIT_RTC:
                    if (btn_event == EVENT_BOTH_LONG) { 
                        edit_field++;
                        if (edit_field > 5) {
                            save_choice = 1; 
                            system_state = STATE_SAVE_PROMPT; 
                        }
                    }
                    else if (btn_event == EVENT_K1_SHORT) { 
                        if(edit_field==0) { edit_time.hour++; if(edit_time.hour>23) edit_time.hour=0; }
                        if(edit_field==1) { edit_time.min++;  if(edit_time.min>59) edit_time.min=0; }
                        if(edit_field==2) { edit_time.sec++;  if(edit_time.sec>59) edit_time.sec=0; } 
                        if(edit_field==3) { edit_time.date++; if(edit_time.date>31) edit_time.date=1; }
                        if(edit_field==4) { edit_time.month++;if(edit_time.month>12) edit_time.month=1; }
                        if(edit_field==5) { edit_time.year++; if(edit_time.year>99) edit_time.year=0; }
                    }
                    else if (btn_event == EVENT_K2_SHORT) { 
                        if(edit_field==0) { if(edit_time.hour==0) edit_time.hour=23; else edit_time.hour--; }
                        if(edit_field==1) { if(edit_time.min==0) edit_time.min=59; else edit_time.min--; }
                        if(edit_field==2) { if(edit_time.sec==0) edit_time.sec=59; else edit_time.sec--; }
                        if(edit_field==3) { if(edit_time.date<=1) edit_time.date=31; else edit_time.date--; }
                        if(edit_field==4) { if(edit_time.month<=1) edit_time.month=12; else edit_time.month--; }
                        if(edit_field==5) { if(edit_time.year==0) edit_time.year=99; else edit_time.year--; }
                    }
                    break;
                    
                case STATE_SAVE_PROMPT:
                    if (btn_event == EVENT_K1_SHORT || btn_event == EVENT_K2_SHORT) {
                        save_choice = !save_choice; 
                    }
                    if (btn_event == EVENT_BOTH_LONG) {
                        if (save_choice == 1) {
                            RTC_SetTime(&edit_time); 
                        }
                        system_state = STATE_PAGE_TIME; 
                    }
                    break;
            }
        }
        
        // 3. System Timers
        if (system_state == STATE_HOME || system_state == STATE_PAGE_TIME) {
            clock_tick++;
            if (clock_tick >= 10) { 
                clock_tick = 0;
                RTC_GetTime(&rtc_time);
                if (rtc_time.sec != last_second) {
                    last_second = rtc_time.sec;
                    update_screen = 1; 
                }
            }
        }
        
        // ==========================================
        // TEXT SCROLL SPEED CONTROLLER
        // ==========================================
        if (system_state == STATE_HOME) {
            scroll_tick++;
            
            // TO CHANGE SPEED: Modify the '30' below.
            // Fast Scroll: if (scroll_tick >= 15) (Shifts every 150ms)
            // Medium Scroll: if (scroll_tick >= 30) (Shifts every 300ms)
            // Slow Scroll: if (scroll_tick >= 50) (Shifts every 500ms / half-second)
            // Very Slow Scroll: if (scroll_tick >= 100) (Shifts every 1 second)
            
            if (scroll_tick >= 50) { 
                scroll_tick = 0;   // Reset the timer
                scroll_pos++;      // Shift the text by 1 letter
                update_screen = 1; // Tell the LCD to redraw
            }
        }
        // ==========================================
        
        if (system_state == STATE_EDIT_RTC) {
            blink_timer++;
            if (blink_timer >= 50) { 
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
                    LCD_SetCursor(1,0); LCD_PRINT("Explorer 16/32  ");
                    __delay_ms(2000); 
                    LCD_CLEAR();
                    system_state = STATE_HOME;
                    break;
                    
                case STATE_HOME:
                    // A) Print the Real-Time Clock on the top line
                    sprintf(lcdBuffer, "Time: %02d:%02d:%02d  ", rtc_time.hour, rtc_time.min, rtc_time.sec);
                    LCD_SetCursor(1,0); LCD_PRINT(lcdBuffer);
                    
                    // B) Scrolling Text Engine on the bottom line
                    {
                        // 1. Define the message. Added extra spaces at the end so it loops cleanly.
                        const char msg[] = "* Made by Dynaspede Integrated Private limited * "; 
                        uint8_t msg_len = strlen(msg);
                        
                        // 2. Keep the tracker within the bounds of the string length
                        if (scroll_pos >= msg_len) scroll_pos = 0;
                        
                        char scroll_buf[17]; // 16 characters for LCD + 1 for Null terminator '\0'
                        
                        // 3. Fill the buffer with 16 letters, starting from the current 'scroll_pos'
                        for(int i = 0; i < 16; i++) {
                            // The '%' (Modulo) operator automatically wraps back to the start 
                            // of the string if it reaches the end, creating a continuous loop.
                            scroll_buf[i] = msg[(scroll_pos + i) % msg_len];
                        }
                        scroll_buf[16] = '\0'; // Always cap off a C-string
                        
                        // 4. Print it
                        LCD_SetCursor(0,0); 
                        LCD_PRINT(scroll_buf);
                    }
                    break;
                    
                case STATE_MENU:
                    {
                        int top_line = menu_index;
                        if (top_line >= NUM_MENU_ITEMS - 1) {
                            top_line = NUM_MENU_ITEMS - 2; 
                        }
                        sprintf(lcdBuffer, "%c%-15s", (menu_index == top_line) ? '>' : ' ', menu_items[top_line]);
                        LCD_SetCursor(0,0); LCD_PRINT(lcdBuffer);
                        sprintf(lcdBuffer, "%c%-15s", (menu_index == top_line+1) ? '>' : ' ', menu_items[top_line+1]);
                        LCD_SetCursor(1,0); LCD_PRINT(lcdBuffer);
                    }
                    break;
                    
                case STATE_PAGE_0:
                case STATE_PAGE_1:
                case STATE_PAGE_2:
                    RTC_GetTime(&rtc_time);
                    sprintf(lcdBuffer, "Time: %02d:%02d:%02d  ", rtc_time.hour, rtc_time.min, rtc_time.sec);
                    LCD_SetCursor(0,0); LCD_PRINT(lcdBuffer);
                    sprintf(lcdBuffer, "Inside Page_%d   ", menu_index);
                    LCD_SetCursor(1,0); LCD_PRINT(lcdBuffer);
                    break;
                    
                case STATE_PAGE_TIME:
                    sprintf(lcdBuffer, "Time: %02d:%02d:%02d  ", rtc_time.hour, rtc_time.min, rtc_time.sec);
                    LCD_SetCursor(0,0); LCD_PRINT(lcdBuffer);
                    sprintf(lcdBuffer, "Date: %02d/%02d/%02d  ", rtc_time.date, rtc_time.month, rtc_time.year);
                    LCD_SetCursor(1,0); LCD_PRINT(lcdBuffer);
                    break;
                    
                case STATE_EDIT_RTC:
                    {
                        char hr[3], mn[3], sc[3], dt[3], mo[3], yr[3];
                        
                        if(edit_field == 0 && !blink_state) strcpy(hr, "  "); else sprintf(hr, "%02d", edit_time.hour);
                        if(edit_field == 1 && !blink_state) strcpy(mn, "  "); else sprintf(mn, "%02d", edit_time.min);
                        if(edit_field == 2 && !blink_state) strcpy(sc, "  "); else sprintf(sc, "%02d", edit_time.sec);
                        if(edit_field == 3 && !blink_state) strcpy(dt, "  "); else sprintf(dt, "%02d", edit_time.date);
                        if(edit_field == 4 && !blink_state) strcpy(mo, "  "); else sprintf(mo, "%02d", edit_time.month);
                        if(edit_field == 5 && !blink_state) strcpy(yr, "  "); else sprintf(yr, "%02d", edit_time.year);

                        sprintf(lcdBuffer, "Time: %s:%s:%s  ", hr, mn, sc);
                        LCD_SetCursor(0,0); LCD_PRINT(lcdBuffer);
                        sprintf(lcdBuffer, "Date: %s/%s/%s  ", dt, mo, yr);
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
        
        // 5. Main Super-Loop Delay
        __delay_ms(10);
    }
    return 1;
}