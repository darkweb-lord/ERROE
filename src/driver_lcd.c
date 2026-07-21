/**
 * @file driver_lcd.c
 * @brief Native I2C LCD Driver (AiP31068) with formatted variadic printing.
 */
#include <xc.h>             
#include <stdio.h>          
#include <stdarg.h>         // Required for variadic arguments (...)
#include "system.h"
#include "driver_lcd.h"
#include "hal_i2c.h"

// Hardware Address (7-bit)
#define LCD_ADDR 0x3E 

// AiP31068 Internal I2C Control Bytes
#define CTRL_CMD  0x00 
#define CTRL_DATA 0x40 

// ============================================================================
// CORE DRIVER FUNCTIONS
// ============================================================================

void LCD_CMD(uint8_t cmd) {
    HAL_I2C1_WriteRegisterBlock(LCD_ADDR, CTRL_CMD, &cmd, 1);
    
    if(cmd == LCD_CMD_CLEAR_DISPLAY || cmd == LCD_CMD_RETURN_HOME) {
        __delay_ms(3); 
    } else {
        __delay_us(50);
    }
}

void LCD_DATA(uint8_t data) {
    HAL_I2C1_WriteRegisterBlock(LCD_ADDR, CTRL_DATA, &data, 1);
    __delay_us(50);
}

void LCD_INIT(void) {
    HAL_I2C1_Init(); 
    __delay_ms(50); 
    
    LCD_CMD(LCD_CMD_FUNC_8BIT_2LINE);
    __delay_ms(5);
    LCD_CMD(LCD_CMD_FUNC_8BIT_2LINE); 
    __delay_us(150);
    
    LCD_CMD(LCD_CMD_DISPLAY_ON_NO_CURS); 
    LCD_CMD(LCD_CMD_CLEAR_DISPLAY);      
    LCD_CMD(LCD_CMD_ENTRY_MODE_LTR);     
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? 0x80 : 0xC0;
    address |= col;
    LCD_CMD(address);
}

void LCD_CLEAR(void) {
    LCD_CMD(LCD_CMD_CLEAR_DISPLAY);
}

void LCD_PRINT(const char* str) {
    while(*str) {
        LCD_DATA(*str++);
    }
}

// ============================================================================
// FORMATTED PRINTING FUNCTION
// ============================================================================

void LCD_PRINTF(uint8_t row, uint8_t col, const char *format, ...) {
    // 16 characters max per line + 1 byte for the null terminator '\0'
    char buffer[17]; 
    
    // Initialize the variable argument list
    va_list args;
    va_start(args, format);
    
    // Safely format the string and variables into the buffer
    // vsnprintf prevents buffer overflows by capping at sizeof(buffer)
    vsnprintf(buffer, sizeof(buffer), format, args);
    
    // Clean up the argument list
    va_end(args);
    
    // Move the hardware cursor and print the final formatted string
    LCD_SetCursor(row, col);
    LCD_PRINT(buffer);
}