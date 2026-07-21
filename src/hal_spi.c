/**
 * @file hal_spi.c
 * @brief SPI1 peripheral control. Configured for Metrology AFE.
 */
#include <xc.h>
#include "system.h"
#include "hal_spi.h"

void HAL_SPI1_Init(void) {
    SPI1CON1 = 0x0000;
    SPI1CON1bits.MSTEN = 1;   
    SPI1CON1bits.CKP = 0;     
    SPI1CON1bits.CKE = 1;     
    SPI1CON1bits.PPRE = 0b11; 
    SPI1CON1bits.SPRE = 0b110;
    SPI1STATbits.SPIEN = 1;   
}

uint8_t SPI1_Exchange(uint8_t data) {
    SPI1BUF = data;
    while(!SPI1STATbits.SPIRBF);
    return SPI1BUF;
}

// Full duplex block transfer. Handles read-only, write-only, and read/write arrays.
void HAL_SPI1_TransferBlock(uint8_t *tx_buf, uint8_t *rx_buf, uint16_t length) {
    for(uint16_t i = 0; i < length; i++) {
        uint8_t tx_val = (tx_buf != NULL) ? tx_buf[i] : 0xFF; 
        uint8_t rx_val = SPI1_Exchange(tx_val);
        if (rx_buf != NULL) {
            rx_buf[i] = rx_val;
        }
    }
}
