/**
 * @file hal_spi.h
 * @brief Scalable SPI transmission layer.
 */
#ifndef HAL_SPI_H
#define HAL_SPI_H

#include <stdint.h>

void HAL_SPI1_Init(void);
void HAL_SPI1_TransferBlock(uint8_t *tx_buf, uint8_t *rx_buf, uint16_t length);

#endif