#include <xc.h>             // Standard XC16 header
#include <stdint.h>         // Required for uint8_t definitions

// 1. You MUST define your clock speed here BEFORE libpic30.h
// Assuming 8MHz internal oscillator (Fosc = 8MHz, FCY = Fosc/2 = 4000000)
// If you enabled the PLL for 32MHz, change this to 16000000UL
#define FCY 16000000UL       

// 2. Delay library included AFTER the speed is defined
#include <libpic30.h>       

// 3. Your local headers
#include "LCD_I2C.h"
#include "I2c_Header.h"

#define LCD_ADDR 0x7C   // 0x3E << 1

void LCD_CMD(uint8_t CMD){
    I2C1_START();
    I2C1_WRITE(LCD_ADDR);
    I2C1_WRITE(0x00);
    I2C1_WRITE(CMD);
    I2C1_STOP();
}

void LCD_DATA(uint8_t DATA){
    I2C1_START();
    I2C1_WRITE(LCD_ADDR);
    I2C1_WRITE(0x40);
    I2C1_WRITE(DATA); //send the ASCII
    I2C1_STOP();
}

void LCD_INIT(void) {
    // 1. Wait for LCD power to stabilize (Critical for AiP31068)
    __delay_ms(50); 

    // 2. Function Set: 8-bit data, 2 lines, 5x8 font
    LCD_CMD(0x38); 
    __delay_ms(1);
    
    // 3. Display ON, Cursor OFF, Blink OFF
    LCD_CMD(0x0C); 
    __delay_ms(1);
    
    // 4. Clear the Display
    LCD_CMD(0x01); 
    __delay_ms(3); // Clearing RAM takes longer, give it 3ms minimum
    
    // 5. Entry Mode Set: Increment cursor to the right automatically
    LCD_CMD(0x06); 
    __delay_ms(1);
}

// Move cursor to a specific Row (0 or 1) and Column (0 to 15)
void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? 0x80 : 0xC0;
    address |= col;
    LCD_CMD(address);
}

// Print a standard C string
void LCD_PRINT(char* str) {
    while(*str) {
        LCD_DATA(*str++);
    }
}