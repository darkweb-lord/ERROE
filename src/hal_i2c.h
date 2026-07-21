/**
 * @file hal_i2c.h
 * @brief Scalable I2C transmission layer with Bus Recovery.
 */
#ifndef HAL_I2C_H
#define HAL_I2C_H

#include <stdint.h>

void HAL_I2C1_BusRecovery(void); // NEW: Bus Recovery Prototype
void HAL_I2C1_Init(void);
uint8_t HAL_I2C1_WriteBlock(uint8_t address, uint8_t *data, uint16_t length);
uint8_t HAL_I2C1_WriteRegisterBlock(uint8_t address, uint8_t reg, uint8_t *data, uint16_t length);
uint8_t HAL_I2C1_ReadBlock(uint8_t address, uint8_t reg, uint8_t *data, uint16_t length);

#endif