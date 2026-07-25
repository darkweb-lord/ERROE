#include<stdint.h>


#ifndef LCD_I2C_H
#define	LCD_I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

void LCD_CMD(uint8_t);
void LCD_DATA(uint8_t);
void LCD_INIT(void);
void LCD_SetCursor(uint8_t , uint8_t );
void LCD_PRINT(char*);
void LCD_CLEAR(void);

void LCD_PRINT_CUSTOME_CHAR(unsigned char location);
void LCD_CREATE_CUSTOM_CHAR(unsigned char location, unsigned char *charmap);


#ifdef	__cplusplus
}
#endif

#endif	/* LCD_I2C_H */

