/**
 * @file app_state_machine.h
 * @brief UI definitions and State Function routing.
 */
#ifndef APP_STATE_MACHINE_H
#define APP_STATE_MACHINE_H

typedef enum {
    STATE_BOOT,
    STATE_HOME,
    STATE_MENU,
    STATE_PAGE_0,
    STATE_PAGE_1,
    STATE_PAGE_2,
    STATE_PAGE_TIME,
    STATE_EDIT_FORMAT, 
    STATE_EDIT_RTC,
    STATE_SAVE_PROMPT
} SystemState_t;

void App_Init(void);
void App_RunStateMachine(void);

#endif