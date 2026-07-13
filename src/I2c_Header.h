#ifndef I2C_HEADER_H
#define I2C_HEADER_H

#ifdef	__cplusplus
extern "C" {
#endif
  
#include <xc.h> // Added to ensure uint8_t and SFRs are known to headers
#include <stdint.h>

void I2C1_BUS_RECOVERY(void);
void I2C_INIT(void);
void I2C1_WAIT(void);
void I2C1_START(void);
void I2C1_RESTART(void);
void I2C1_STOP(void);
uint8_t I2C1_WRITE(uint8_t data);
uint8_t I2C1_READ(uint8_t ack); // 0 = ACK, 1 = NACK


#ifdef	__cplusplus
}
#endif

#endif /* I2C_HEADER_H */ 
// Removed the extra #endif that was on line 21
