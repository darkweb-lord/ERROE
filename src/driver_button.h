/**
 * @file driver_button.h
 * @brief Key matrix reading.
 */
#ifndef DRIVER_BUTTON_H
#define DRIVER_BUTTON_H
#include <stdint.h>

typedef enum { 
    EVENT_NONE, 
    EVENT_K1_SHORT,
    EVENT_K2_SHORT, 
    EVENT_BOTH_LONG 
} ButtonEvent_t;

ButtonEvent_t Button_Read_Matrix(void);

#endif