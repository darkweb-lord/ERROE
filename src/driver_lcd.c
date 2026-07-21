/**
 * @file driver_lcd.c
 * @brief Uses PCF8574 Backpack over I2C to control 16x2 Display.
 */
#include <xc.h>             
#include "system.h"
#include "driver_lcd.h"
#include "hal_i2c.h"

// 7-bit Address. HAL automatically left-shifts it to 0x7C.
#define LCD_ADDR 0x3E 
uint8_t backlight_state = 0x08; 

static void LCD_I2C_Write(uint8_t data) {
    HAL_I2C1_WriteBlock(LCD_ADDR, &data, 1);
}

static void LCD_Pulse_E(uint8_t data) {
    LCD_I2C_Write(data | 0x04 | backlight_state); 
    __delay_us(50);
    LCD_I2C_Write((data & ~0x04) | backlight_state); 
    __delay_us(50);
}

static void LCD_SendNibble(uint8_t nibble, uint8_t rs) {
    uint8_t data = (nibble << 4) | (rs ? 0x01 : 0x00) | backlight_state;
    LCD_I2C_Write(data);
    LCD_Pulse_E(data);
}

static void LCD_CMD(uint8_t cmd) {
    LCD_SendNibble(cmd >> 4, 0);   
    LCD_SendNibble(cmd & 0x0F, 0); 
    if(cmd == 0x01 || cmd == 0x02) __delay_ms(3); 
}

static void LCD_DATA(uint8_t data) {
    LCD_SendNibble(data >> 4, 1);   
    LCD_SendNibble(data & 0x0F, 1); 
}

void LCD_INIT(void) {
    HAL_I2C1_Init(); 
    __delay_ms(50); 
    
    LCD_SendNibble(0x03, 0); __delay_ms(5);
    LCD_SendNibble(0x03, 0); __delay_us(150);
    LCD_SendNibble(0x03, 0); __delay_us(150);
    LCD_SendNibble(0x02, 0); __delay_us(150);
    
    LCD_CMD(0x28); LCD_CMD(0x08); LCD_CMD(0x01); 
    LCD_CMD(0x06); LCD_CMD(0x0C); 
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? 0x80 : 0xC0;
    address |= col;
    LCD_CMD(address);
}

void LCD_PRINT(const char* str) {
    while(*str) LCD_DATA(*str++);
}

void LCD_CLEAR(void) {
    LCD_CMD(0x01);
    __delay_ms(3);
}