/**
 * @file driver_lcd.h
 * @brief Native I2C LCD driver for AiP31068 controller with formatted printing.
 */
#ifndef DRIVER_LCD_H
#define	DRIVER_LCD_H

#include <stdint.h>

// ============================================================================
// LCD COMMAND DEFINITIONS (AiP31068 / HD44780 Instruction Set)
// ============================================================================
#define LCD_CMD_CLEAR_DISPLAY       0x01 
#define LCD_CMD_RETURN_HOME         0x02 
#define LCD_CMD_ENTRY_MODE_RTL      0x04 
#define LCD_CMD_ENTRY_MODE_LTR      0x06 
#define LCD_CMD_DISPLAY_OFF         0x08 
#define LCD_CMD_DISPLAY_ON_NO_CURS  0x0C 
#define LCD_CMD_DISPLAY_ON_CURSOR   0x0E 
#define LCD_CMD_DISPLAY_ON_BLINK    0x0F 
#define LCD_CMD_SHIFT_CURS_LEFT     0x10 
#define LCD_CMD_SHIFT_CURS_RIGHT    0x14 
#define LCD_CMD_SHIFT_DISP_LEFT     0x18 
#define LCD_CMD_SHIFT_DISP_RIGHT    0x1C 
#define LCD_CMD_FUNC_8BIT_2LINE     0x38 

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================
void LCD_INIT(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_CLEAR(void);

// Text Printing
void LCD_PRINT(const char* str);

/**
 * @brief Prints formatted text and variables directly to a specific LCD row and column.
 * @param row The LCD row (0 or 1)
 * @param col The starting column (0 to 15)
 * @param format The format string (standard printf formatting)
 * @param ... Variable arguments matching the format string
 */
void LCD_PRINTF(uint8_t row, uint8_t col, const char *format, ...);

#endif	/* DRIVER_LCD_H */