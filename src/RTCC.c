//Real-Time Clock and Calendar.- RTCC
//IC - MCP79412


#include "RTCC.h"
#include "I2c_Header.h"

#define RTC_WRITE_ADDR 0xDE
#define RTC_READ_ADDR  0xDF

// Helper Math Functions for BCD conversion
uint8_t DecToBcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

uint8_t BcdToDec(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

void RTC_Init(void) {
    // We do a dummy read to check if oscillator is already running.
    // If you are setting the time later, the ST bit will be set automatically.
    uint8_t sec_reg;

    // --- STEP 1: Point to the Seconds Register (0x00) ---
    I2C1_START();
    I2C1_WRITE(RTC_WRITE_ADDR);
    I2C1_WRITE(0x00);
    
    // --- STEP 2: Read the Seconds Register ---
    I2C1_RESTART();
    I2C1_WRITE(RTC_READ_ADDR);
    sec_reg = I2C1_READ(1); // 1 = Send NACK because we only want 1 byte
    I2C1_STOP();

    // --- STEP 3: Check the ST Bit (Bit 7) ---
    // We use the bitwise AND mask (0x80 or 1000 0000 in binary) to check only bit 7.
    if ((sec_reg & 0x80) == 0) {
        
        // The result is 0, meaning the clock is stopped!
        // We must write the register back and force the ST bit to 1.
        
        I2C1_START();
        I2C1_WRITE(RTC_WRITE_ADDR);
        I2C1_WRITE(0x00);
        
        // Use bitwise OR (| 0x80) to keep the current seconds but set Bit 7 to 1
        I2C1_WRITE(sec_reg | 0x80); 
        
        I2C1_STOP();
    }
}


void RTC_SetTime(RTC_TIME_t *t) {
    I2C1_START();
    I2C1_WRITE(RTC_WRITE_ADDR);
    I2C1_WRITE(0x00); // Start at Register 0x00 (Seconds)
    
    // Write Time (And ensure ST bit 7 is '1' to start oscillator)
    I2C1_WRITE(DecToBcd(t->sec) | 0x80); 
    I2C1_WRITE(DecToBcd(t->min));
    I2C1_WRITE(DecToBcd(t->hour)); // 24-hour mode by default (bit 6 = 0)
    
    // Write Date (And ensure VBATEN bit 3 is '1' to enable Battery Backup)
    I2C1_WRITE(DecToBcd(t->day) | 0x08); 
    I2C1_WRITE(DecToBcd(t->date));
    I2C1_WRITE(DecToBcd(t->month));
    I2C1_WRITE(DecToBcd(t->year));
    
    I2C1_STOP();
}

void RTC_GetTime(RTC_TIME_t *t) {
    I2C1_START();
    I2C1_WRITE(RTC_WRITE_ADDR);
    I2C1_WRITE(0x00); // Point to Register 0x00
    
    I2C1_RESTART();   // Change bus direction to READ
    I2C1_WRITE(RTC_READ_ADDR);
    
    // Read registers and mask out the control bits
    t->sec   = BcdToDec(I2C1_READ(0) & 0x7F); // 0 = Send ACK
    t->min   = BcdToDec(I2C1_READ(0) & 0x7F);
    t->hour  = BcdToDec(I2C1_READ(0) & 0x3F);
    t->day   = BcdToDec(I2C1_READ(0) & 0x07);
    t->date  = BcdToDec(I2C1_READ(0) & 0x3F);
    t->month = BcdToDec(I2C1_READ(0) & 0x1F);
    t->year  = BcdToDec(I2C1_READ(1));        // 1 = Send NACK (Last byte)
    
    I2C1_STOP();
}
