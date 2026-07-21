/**
 * @file app_state_machine.c
 * @brief UI logic implementation utilizing clean LCD_PRINTF variadic wrappers.
 */
#include "system.h"
#include "app_state_machine.h"
#include "driver_lcd.h"
#include "driver_rtcc.h"
#include "driver_button.h"
#include "driver_atsense.h"

SystemState_t current_state = STATE_BOOT;
RTC_TIME_t rtc_time;
uint8_t update_screen = 1;

#define NUM_MENU_ITEMS 5
const char* menu_items[NUM_MENU_ITEMS] = {"1.Page_0", "2.Page_1", "3.Page_2", "4.Time/Date", "5.Exit Menu"};
int8_t menu_index = 0;

uint8_t last_second = 60;
uint16_t scroll_tick = 0, scroll_pos = 0;

RTC_TIME_t edit_time;
uint8_t edit_field = 0, save_choice = 1, blink_state = 1;
uint16_t blink_timer = 0;
uint8_t temp_format = 0;

// --- STATE HANDLERS ---

void State_Boot(ButtonEvent_t btn) {
    if (update_screen) {
        LCD_PRINTF(0, 0, "System Booting..");
        LCD_PRINTF(1, 0, "Made by DISPL   ");
        update_screen = 0;
    }
    __delay_ms(2000);
    LCD_CLEAR();
    current_state = STATE_HOME;
    update_screen = 1;
}

void State_Home(ButtonEvent_t btn) {
    if (btn == EVENT_BOTH_LONG) {
        current_state = STATE_MENU;
        update_screen = 1;
        return;
    }

    RTC_GetTime(&rtc_time);
    if (rtc_time.sec != last_second) {
        last_second = rtc_time.sec;
        update_screen = 1;
    }

    scroll_tick++;
    if (scroll_tick >= 50) {
        scroll_tick = 0;
        scroll_pos++;
        update_screen = 1;
    }

    if (update_screen) {
        char am_pm[3] = "  ";
        if (rtc_time.format_12h) strcpy(am_pm, rtc_time.is_pm ? "PM" : "AM");

        // CLEAN SYNTAX: Print Time directly
        LCD_PRINTF(0, 0, "Time: %02d:%02d:%02d%s", rtc_time.hour, rtc_time.min, rtc_time.sec, am_pm);

        // Scrolling Text Logic
        const char msg[] = "-* MADE BY DYNASPEDE INTERGRATED PRIVATE LIMITED *";
        uint8_t msg_len = strlen(msg);
        if (scroll_pos >= msg_len) scroll_pos = 0;

        char scroll_buf[17];
        for (int i = 0; i < 16; i++) scroll_buf[i] = msg[(scroll_pos + i) % msg_len];
        scroll_buf[16] = '\0';

        LCD_PRINTF(1, 0, "%s", scroll_buf);
        update_screen = 0;
    }
}

void State_Menu(ButtonEvent_t btn) {
    if (btn == EVENT_K1_SHORT) {
        menu_index++;
        if (menu_index >= NUM_MENU_ITEMS) menu_index = 0;
        update_screen = 1;
    }
    if (btn == EVENT_K2_SHORT) {
        menu_index--;
        if (menu_index < 0) menu_index = NUM_MENU_ITEMS - 1;
        update_screen = 1;
    }
    if (btn == EVENT_BOTH_LONG) {
        if (menu_index == 0) current_state = STATE_PAGE_0;
        if (menu_index == 1) current_state = STATE_PAGE_1;
        if (menu_index == 2) current_state = STATE_PAGE_2;
        if (menu_index == 3) current_state = STATE_PAGE_TIME;
        if (menu_index == 4) {
            current_state = STATE_HOME;
            menu_index = 0;
            LCD_CLEAR();
        }
        update_screen = 1;
    }

    if (update_screen) {
        int top_line = menu_index;
        if (top_line >= NUM_MENU_ITEMS - 1) top_line = NUM_MENU_ITEMS - 2;

        // CLEAN SYNTAX: Dynamic Menu Rendering
        LCD_PRINTF(0, 0, "%c%-15s", (menu_index == top_line) ? '>' : ' ', menu_items[top_line]);
        LCD_PRINTF(1, 0, "%c%-15s", (menu_index == top_line + 1) ? '>' : ' ', menu_items[top_line + 1]);
        update_screen = 0;
    }
}

void State_Page_0(ButtonEvent_t btn) {
    if (btn == EVENT_BOTH_LONG) {
        current_state = STATE_MENU;
        update_screen = 1;
    }
    if (update_screen) {
        LCD_PRINTF(0, 0, "Inside Page_0   ");
        LCD_PRINTF(1, 0, "Hold BOTH-> Back");
        update_screen = 0;
    }
    /*if (btn == EVENT_BOTH_LONG) {
        current_state = STATE_MENU;
        update_screen = 1;
    }
    if (update_screen) {
        // CLEAN SYNTAX: Printing raw 32-bit Metrology data
        LCD_PRINTF(0, 0, "VA_Raw: %-8ld", meter_data.v_a);
        LCD_PRINTF(1, 0, "Hold BOTH-> Back");
        update_screen = 0;
    } */
}

void State_Page_1(ButtonEvent_t btn) {
    if (btn == EVENT_BOTH_LONG) {
        current_state = STATE_MENU;
        update_screen = 1;
    }
    if (update_screen) {
        LCD_PRINTF(0, 0, "Inside Page_1   ");
        LCD_PRINTF(1, 0, "Hold BOTH-> Back");
        update_screen = 0;
    }
}

void State_Page_2(ButtonEvent_t btn) {
    if (btn == EVENT_BOTH_LONG) {
        current_state = STATE_MENU;
        update_screen = 1;
    }
    if (update_screen) {
        LCD_PRINTF(0, 0, "Inside Page_2   ");
        LCD_PRINTF(1, 0, "Hold BOTH-> Back");
        update_screen = 0;
    }
}

void State_Page_Time(ButtonEvent_t btn) {
    if (btn == EVENT_BOTH_LONG) {
        RTC_GetTime(&edit_time);
        temp_format = edit_time.format_12h;
        current_state = STATE_EDIT_FORMAT;
        update_screen = 1;
    }
    if (btn == EVENT_K1_SHORT || btn == EVENT_K2_SHORT) {
        current_state = STATE_MENU;
        update_screen = 1;
    }

    RTC_GetTime(&rtc_time);
    if (rtc_time.sec != last_second) {
        last_second = rtc_time.sec;
        update_screen = 1;
    }
        update_screen = 0;

    if (update_screen) {
        char am_pm[3] = "  ";
        if (rtc_time.format_12h) strcpy(am_pm, rtc_time.is_pm ? "PM" : "AM");

        // CLEAN SYNTAX: Rendering Time and Date
        LCD_PRINTF(0, 0, "Time: %02d:%02d:%02d%s", rtc_time.hour, rtc_time.min, rtc_time.sec, am_pm);
        LCD_PRINTF(1, 0, "Date: %02d/%02d/%02d  ", rtc_time.date, rtc_time.month, rtc_time.year);
    }
}

void State_Edit_Format(ButtonEvent_t btn) {
    if (btn == EVENT_K1_SHORT || btn == EVENT_K2_SHORT) {
        temp_format = !temp_format;
        update_screen = 1;
    }
    if (btn == EVENT_BOTH_LONG) {
        if (temp_format != edit_time.format_12h) {
            if (temp_format == 1) {
                if (edit_time.hour == 0) {
                    edit_time.hour = 12;
                    edit_time.is_pm = 0;
                } else if (edit_time.hour == 12) {
                    edit_time.is_pm = 1;
                } else if (edit_time.hour > 12) {
                    edit_time.hour -= 12;
                    edit_time.is_pm = 1;
                } else {
                    edit_time.is_pm = 0;
                }
            } else {
                if (edit_time.is_pm && edit_time.hour < 12) edit_time.hour += 12;
                if (!edit_time.is_pm && edit_time.hour == 12) edit_time.hour = 0;
                edit_time.is_pm = 0;
            }
            edit_time.format_12h = temp_format;
        }
        edit_field = 0;
        current_state = STATE_EDIT_RTC;
        update_screen = 1;
    }

    if (update_screen) {
        LCD_PRINTF(0, 0, "Set Time Format:");
        if (temp_format == 1) {
            LCD_PRINTF(1, 0, ">12-Hour 24-Hour");
        }
        else {
            LCD_PRINTF(1, 0, " 12-Hour>24-Hour");
        }
        update_screen = 0;
    }
}

void State_Edit_RTC(ButtonEvent_t btn) {
    if (btn == EVENT_BOTH_LONG) {
        edit_field++;
        if (edit_time.format_12h == 0 && edit_field == 3) edit_field = 4;
        if (edit_field > 6) {
            save_choice = 1;
            current_state = STATE_SAVE_PROMPT;
        }
        update_screen = 1;
    } else if (btn == EVENT_K1_SHORT) {
        if (edit_field == 0) {
            if (edit_time.format_12h) {
                edit_time.hour++;
                if (edit_time.hour > 12) edit_time.hour = 1;
            } else {
                edit_time.hour++;
                if (edit_time.hour > 23) edit_time.hour = 0;
            }
        }
        if (edit_field == 1) {
            edit_time.min++;
            if (edit_time.min > 59) edit_time.min = 0;
        }
        if (edit_field == 2) {
            edit_time.sec++;
            if (edit_time.sec > 59) edit_time.sec = 0;
        }
        if (edit_field == 3) {
            edit_time.is_pm = !edit_time.is_pm;
        }
        if (edit_field == 4) {
            edit_time.date++;
            if (edit_time.date > 31) edit_time.date = 1;
        }
        if (edit_field == 5) {
            edit_time.month++;
            if (edit_time.month > 12) edit_time.month = 1;
        }
        if (edit_field == 6) {
            edit_time.year++;
            if (edit_time.year > 99) edit_time.year = 0;
        }
        update_screen = 1;
    } else if (btn == EVENT_K2_SHORT) {
        if (edit_field == 0) {
            if (edit_time.format_12h) {
                edit_time.hour--;
                if (edit_time.hour < 1) edit_time.hour = 12;
            } else {
                if (edit_time.hour == 0) edit_time.hour = 23;
                else edit_time.hour--;
            }
        }
        if (edit_field == 1) {
            if (edit_time.min == 0) edit_time.min = 59;
            else edit_time.min--;
        }
        if (edit_field == 2) {
            if (edit_time.sec == 0) edit_time.sec = 59;
            else edit_time.sec--;
        }
        if (edit_field == 3) {
            edit_time.is_pm = !edit_time.is_pm;
        }
        if (edit_field == 4) {
            if (edit_time.date <= 1) edit_time.date = 31;
            else edit_time.date--;
        }
        if (edit_field == 5) {
            if (edit_time.month <= 1) edit_time.month = 12;
            else edit_time.month--;
        }
        if (edit_field == 6) {
            if (edit_time.year == 0) edit_time.year = 99;
            else edit_time.year--;
        }
        update_screen = 1;
    }

    blink_timer++;
    if (blink_timer >= 50) {
        blink_timer = 0;
        blink_state = !blink_state;
        update_screen = 1;
    }

    if (update_screen) {
        char hr[3], mn[3], sc[3], ap[3], dt[3], mo[3], yr[3];

        if (edit_field == 0 && !blink_state) strcpy(hr, "  ");
        else sprintf(hr, "%02d", edit_time.hour);
        if (edit_field == 1 && !blink_state) strcpy(mn, "  ");
        else sprintf(mn, "%02d", edit_time.min);
        if (edit_field == 2 && !blink_state) strcpy(sc, "  ");
        else sprintf(sc, "%02d", edit_time.sec);

        if (edit_time.format_12h) {
            if (edit_field == 3 && !blink_state) strcpy(ap, "  ");
            else strcpy(ap, edit_time.is_pm ? "PM" : "AM");
        } else {
            strcpy(ap, "  ");
        }

        if (edit_field == 4 && !blink_state) strcpy(dt, "  ");
        else sprintf(dt, "%02d", edit_time.date);
        if (edit_field == 5 && !blink_state) strcpy(mo, "  ");
        else sprintf(mo, "%02d", edit_time.month);
        if (edit_field == 6 && !blink_state) strcpy(yr, "  ");
        else sprintf(yr, "%02d", edit_time.year);

        // CLEAN SYNTAX: Blinking Edit Fields
        LCD_PRINTF(0, 0, "Time: %s:%s:%s%s", hr, mn, sc, ap);
        LCD_PRINTF(1, 0, "Date: %s/%s/%s  ", dt, mo, yr);
        update_screen = 0;
    }
}

void State_Save_Prompt(ButtonEvent_t btn) {
    if (btn == EVENT_K1_SHORT || btn == EVENT_K2_SHORT) {
        save_choice = !save_choice;
        update_screen = 1;
    }
    if (btn == EVENT_BOTH_LONG) {
        if (save_choice == 1) RTC_SetTime(&edit_time);
        current_state = STATE_PAGE_TIME;
        update_screen = 1;
    }

    if (update_screen) {
        LCD_PRINTF(0, 0, "Save Changes?   ");
        if (save_choice == 1) {
            LCD_PRINTF(1, 0, ">YES     NO     ");
        }
        else {
            LCD_PRINTF(1, 0, " YES    >NO     ");
        }
        update_screen = 0;
    }
}

// --- FUNCTION POINTER ARRAY ---
typedef void (*StateFunc_t)(ButtonEvent_t);
StateFunc_t StateTable[] = {
    State_Boot, // 0
    State_Home, // 1
    State_Menu, // 2
    State_Page_0, // 3
    State_Page_1, // 4
    State_Page_2, // 5
    State_Page_Time, // 6
    State_Edit_Format, // 7
    State_Edit_RTC, // 8
    State_Save_Prompt // 9
};

void App_Init(void) {
    LCD_INIT();
    RTC_Init();
    ATSENSE_Init();
}

void App_RunStateMachine(void) {
    ButtonEvent_t btn_event = Button_Read_Matrix();
    if (new_data_ready) {
        new_data_ready = 0;
        ATSENSE_FetchData();
    }
    StateTable[current_state](btn_event);
}