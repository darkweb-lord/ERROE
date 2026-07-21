/**
 * @file driver_eeprom.c
 * @brief Implementation for dual-EEPROM architecture.
 */
#include "system.h"        // MUST BE FIRST! Defines FCY and includes <libpic30.h> safely
#include "driver_eeprom.h"
#include "hal_i2c.h"

// The EEPROM block on the MCP79412 uses a separate I2C Device Address (0x57)
#define EEPROM_RTC_ADDR 0x57 

// Fallback macro definition for PIC24F Data EEPROM base address
#ifndef __EEDATA_BASE
#define __EEDATA_BASE 0x7FFE00UL
#endif

// ============================================================================
// 1. EXTERNAL RTC EEPROM (MCP79412)
// ============================================================================

void EEPROM_RTC_WriteByte(uint8_t address, uint8_t data) {
    HAL_I2C1_WriteRegisterBlock(EEPROM_RTC_ADDR, address, &data, 1);
    __delay_ms(5); // 5ms internal write cycle delay per datasheet
}

uint8_t EEPROM_RTC_ReadByte(uint8_t address) {
    uint8_t data = 0;
    HAL_I2C1_ReadBlock(EEPROM_RTC_ADDR, address, &data, 1);
    return data;
}

void EEPROM_RTC_WriteBlock(uint8_t start_address, uint8_t *data, uint16_t length) {
    // Write byte-by-byte to safely cross page boundaries
    for (uint16_t i = 0; i < length; i++) {
        EEPROM_RTC_WriteByte(start_address + i, data[i]);
    }
}

void EEPROM_RTC_ReadBlock(uint8_t start_address, uint8_t *data, uint16_t length) {
    HAL_I2C1_ReadBlock(EEPROM_RTC_ADDR, start_address, data, length);
}


// ============================================================================
// 2. INTERNAL MCU EEPROM (PIC24FV32KA304)
// ============================================================================

void EEPROM_MCU_WriteWord(uint16_t word_offset, uint16_t data) {
    // Calculate absolute EEPROM memory address
    // Offset is multiplied by 2 because program memory space is byte-addressed
    _prog_addressT eeprom_addr = __EEDATA_BASE + (word_offset * 2);
    
    // 1. Erase the existing 16-bit word
    _erase_eedata(eeprom_addr, _EE_WORD);
    _wait_eedata(); 
    
    // 2. Write the new 16-bit word
    _write_eedata_word(eeprom_addr, data);
    _wait_eedata(); 
}

uint16_t EEPROM_MCU_ReadWord(uint16_t word_offset) {
    _prog_addressT eeprom_addr = __EEDATA_BASE + (word_offset * 2);
    
    // Read word directly from EEPROM address space using XC16 helper
    uint16_t data = 0;
    _memcpy_p2d16(&data, eeprom_addr, 2);
    return data;
}