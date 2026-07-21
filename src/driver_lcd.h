/**
 * @file driver_lcd.h
 * @brief I2C Alphanumeric LCD driver.
 */
#ifndef DRIVER_LCD_H
#define	DRIVER_LCD_H

#include <stdint.h>

void LCD_INIT(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_PRINT(const char* str);
void LCD_CLEAR(void);

#endif