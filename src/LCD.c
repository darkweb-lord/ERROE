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

//0x4E <--(0x27 << 1)  //0b 0100 1110 <--(0b 0010 0111 << 1) //78 <--(27 << 1)
//0x7C <--(0x3E << 1)  //0b 0111 1100 <--(0b 0011 1110 << 1) //124 <--(62 << 1)

/* FIX C4: datasheet-correct AiP31068 write address (was 0x4E) */
#define LCD_ADDR   0x7C          /* 0x3E<<1 ; use 0x4C only if a PCF8574 @0x26 is confirmed */
#define LCD_CTRL_CMD   0x00      /* control byte: next byte is a command (RS=0) */
#define LCD_CTRL_DATA  0x40      /* control byte: next byte is data    (RS=1) */

/* returns 0 on success, 1 if slave did not ACK (FIX C5) */
static uint8_t LCD_XFER(uint8_t control, uint8_t value)
{
    uint8_t nack = 0;
    I2C1_START();
    nack |= I2C1_WRITE(LCD_ADDR);      /* ACKSTAT: 0=ACK, 1=NACK */
    nack |= I2C1_WRITE(control);
    nack |= I2C1_WRITE(value);
    I2C1_STOP();
    return nack;
}

void LCD_CMD(uint8_t cmd)  { LCD_XFER(LCD_CTRL_CMD,  cmd);  }
void LCD_DATA(uint8_t d)   { LCD_XFER(LCD_CTRL_DATA, d);    }

void LCD_INIT(void) {
    // 1. Wait for LCD power to stabilize (Critical for AiP31068)
    __delay_ms(50); 
    // 2. Function Set: 8-bit data, 2 lines, 5x8 font
    LCD_CMD(0x38); __delay_ms(5);
    LCD_CMD(0x38); __delay_us(150);
    // 3. Display ON, Cursor OFF, Blink OFF
    LCD_CMD(0x0C); __delay_ms(2);
    // 4. Clear the Display
    LCD_CMD(0x01); __delay_ms(2); // Clearing RAM takes longer, give it 3ms minimum
    // 5. Entry Mode Set: Increment cursor to the right automatically
    LCD_CMD(0x06); __delay_ms(2);
}

// Move cursor to a specific Row (0 or 1) and Column (0 to 15)
void LCD_SetCursor(uint8_t row, uint8_t col)
{
    static const uint8_t off[2] = {0x00, 0x40};
    LCD_CMD(0x80 | (off[row & 1] + col));
}

// Print a standard C string
void LCD_PRINT(char* str) { while (*str) LCD_DATA((uint8_t)*str++);  }

// Clear the LCD screen
void LCD_CLEAR(void){ LCD_CMD(0x01); __delay_ms(3); }