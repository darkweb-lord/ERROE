#ifndef SPI1_H
#define SPI1_H

#include <xc.h>
#include <stdint.h>

// ==========================================
// ATSENSE301-H Hardware Pin Macros
// ==========================================
#define ATSENSE_CS_LAT    LATAbits.LATA9  // PIN 35 (RA9): chip select
#define ATSENSE_MCLR_LAT  LATBbits.LATB4  // PIN 33 (RB4): meteringMclr
#define ATSENSE_INT_PORT  PORTAbits.RA4   // PIN 34 (RA4): metering_interrupt

// ==========================================
// Function Prototypes
// ==========================================
void SPI1_Initialize(void);
void ATSENSE_HardwareReset(void);
uint8_t SPI1_Exchange8bit(uint8_t data);
void ATSENSE_WriteRegister(uint8_t reg_address, uint32_t data);
uint32_t ATSENSE_ReadRegister(uint8_t reg_address);

#endif /* SPI1_H */