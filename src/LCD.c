#include <xc.h>             
#include <stdint.h>         

// MUST define clock speed before libpic30.h
#define FCY 16000000UL       
#include <libpic30.h>       

#include "LCD.h"

// --- Hardware Mapping: MA240022 PIM to Explorer 16/32 LCD ---
#define LCD_RS LATBbits.LATB9
#define LCD_RW LATBbits.LATB8
#define LCD_E  LATBbits.LATB7

#define LCD_D4 LATCbits.LATC4
#define LCD_D5 LATCbits.LATC5
#define LCD_D6 LATCbits.LATC6
#define LCD_D7 LATCbits.LATC7

#define LCD_RS_TRIS TRISBbits.TRISB9
#define LCD_RW_TRIS TRISBbits.TRISB8
#define LCD_E_TRIS  TRISBbits.TRISB7

#define LCD_D4_TRIS TRISCbits.TRISC4
#define LCD_D5_TRIS TRISCbits.TRISC5
#define LCD_D6_TRIS TRISCbits.TRISC6
#define LCD_D7_TRIS TRISCbits.TRISC7

// Create the 'Enable' latch pulse
void LCD_Pulse_E(void) {
    LCD_E = 1;
    __delay_us(2);
    LCD_E = 0;
    __delay_us(50);
}

// Send 4 bits to the hardware bus
void LCD_SendNibble(uint8_t nibble) {
    LCD_D4 = (nibble & 0x01) ? 1 : 0;
    LCD_D5 = (nibble & 0x02) ? 1 : 0;
    LCD_D6 = (nibble & 0x04) ? 1 : 0;
    LCD_D7 = (nibble & 0x08) ? 1 : 0;
    LCD_Pulse_E();
}

void LCD_CMD(uint8_t cmd) {
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_SendNibble(cmd >> 4);   // Send High Nibble
    LCD_SendNibble(cmd & 0x0F); // Send Low Nibble
    if(cmd == 0x01 || cmd == 0x02) {
        __delay_ms(3); // Clear/Home commands need more time
    }
}

void LCD_DATA(uint8_t data) {
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_SendNibble(data >> 4);
    LCD_SendNibble(data & 0x0F);
}

void LCD_INIT(void) {
    // 1. Configure all mapped pins as digital outputs
    LCD_RS_TRIS = 0;
    LCD_RW_TRIS = 0;
    LCD_E_TRIS  = 0;
    LCD_D4_TRIS = 0;
    LCD_D5_TRIS = 0;
    LCD_D6_TRIS = 0;
    LCD_D7_TRIS = 0;
    
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_E  = 0;

    __delay_ms(50); // Wait for physical LCD power to stabilize
    
    // 2. 4-bit Initialization Sequence
    LCD_SendNibble(0x03);
    __delay_ms(5);
    LCD_SendNibble(0x03);
    __delay_us(150);
    LCD_SendNibble(0x03);
    __delay_us(150);
    LCD_SendNibble(0x02); // Set to 4-bit mode
    __delay_us(150);
    
    // 3. Configure LCD Parameters
    LCD_CMD(0x28); // Function Set: 4-bit, 2 lines, 5x8 font
    LCD_CMD(0x08); // Display off
    LCD_CMD(0x01); // Clear display
    LCD_CMD(0x06); // Entry mode: Increment right
    LCD_CMD(0x0C); // Display ON, Cursor OFF, Blink OFF
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? 0x80 : 0xC0;
    address |= col;
    LCD_CMD(address);
}

void LCD_PRINT(char* str) {
    while(*str) {
        LCD_DATA(*str++);
    }
}

void LCD_CLEAR(void) {
    LCD_CMD(0x01);
    __delay_ms(3);
}