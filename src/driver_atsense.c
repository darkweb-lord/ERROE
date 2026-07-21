/**
 * @file driver_atsense.c
 * @brief Reads 24-bit raw ADC registers via SPI triggered by DRDY interrupt.
 */
#include <xc.h>
#include "system.h"
#include "driver_atsense.h"
#include "hal_spi.h"

#define ATSENSE_CS LATBbits.LATB10
#define DRDY_TRIS  TRISBbits.TRISB11

MetrologyData_t meter_data;
volatile uint8_t new_data_ready = 0;

static uint32_t ATSENSE_ReadRegister(uint8_t reg) {
    uint8_t rx[3] = {0};
    uint8_t cmd = reg & 0x7F; // Bit 7 low for Read
    
    ATSENSE_CS = 0;
    HAL_SPI1_TransferBlock(&cmd, NULL, 1);
    HAL_SPI1_TransferBlock(NULL, rx, 3);
    ATSENSE_CS = 1;
    
    uint32_t val = ((uint32_t)rx[0]<<16) | ((uint32_t)rx[1]<<8) | rx[2];
    if (val & 0x00800000) val |= 0xFF000000; // Sign Extension
    return val;
}

void ATSENSE_Init(void) {
    TRISBbits.TRISB10 = 0; 
    ATSENSE_CS = 1;
    HAL_SPI1_Init();
    
    DRDY_TRIS = 1;  
    INTCON2bits.INT1EP = 1; // Interrupt on Falling Edge
    IFS1bits.INT1IF = 0;
    IEC1bits.INT1IE = 1;
}

void ATSENSE_FetchData(void) {
    meter_data.v_a = ATSENSE_ReadRegister(0x01);
    meter_data.v_b = ATSENSE_ReadRegister(0x02);
    meter_data.v_c = ATSENSE_ReadRegister(0x03);
    meter_data.i_a = ATSENSE_ReadRegister(0x04);
    meter_data.i_b = ATSENSE_ReadRegister(0x05);
    meter_data.i_c = ATSENSE_ReadRegister(0x06);
    meter_data.i_n = ATSENSE_ReadRegister(0x07);
}

void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void) {
    IFS1bits.INT1IF = 0;
    new_data_ready = 1;
}
