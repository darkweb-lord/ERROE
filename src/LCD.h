#ifndef LCD_H
#define	LCD_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void LCD_INIT(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_PRINT(char* str);
void LCD_CLEAR(void);

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */