/**
 * @file driver_eeprom.h
 * @brief Scalable driver for both External RTC EEPROM and Internal MCU EEPROM.
 */
#ifndef DRIVER_EEPROM_H
#define DRIVER_EEPROM_H

#include <stdint.h>

// ============================================================================
// 1. EXTERNAL RTC EEPROM (MCP79412) - BEST FOR REPETITIVE DATA LOGGING
// Capacity: 128 Bytes (Addresses: 0x00 to 0x7F)
// Endurance: 1,000,000 Cycles | Access: 8-bit Byte
// ============================================================================

void EEPROM_RTC_WriteByte(uint8_t address, uint8_t data);
uint8_t EEPROM_RTC_ReadByte(uint8_t address);
void EEPROM_RTC_WriteBlock(uint8_t start_address, uint8_t *data, uint16_t length);
void EEPROM_RTC_ReadBlock(uint8_t start_address, uint8_t *data, uint16_t length);


// ============================================================================
// 2. INTERNAL MCU EEPROM (PIC24F) - BEST FOR STATIC CALIBRATION/CONFIG
// Capacity: 2 KBytes 
// Endurance: 100,000 Cycles | Access: 16-bit Word
// ============================================================================

void EEPROM_MCU_WriteWord(uint16_t word_offset, uint16_t data);
uint16_t EEPROM_MCU_ReadWord(uint16_t word_offset);

#endif /* DRIVER_EEPROM_H */