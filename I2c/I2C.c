#include <xc.h>         // This loads the p24FV32KA304.h definitions automatically
#include "I2c_Header.h"

void I2C_INIT(void){
    I2C1CON = 0x0000;
    I2C1STAT =0x0000;
    
    I2C1BRG = 39 ; // 157 SCL operate at 100KHz on 16MHz 
    I2C1CONbits.DISSLW = 1;
    IFS1bits.MI2C1IF = 0;  // Master Interrupt Flag
    I2C1CONbits.I2CEN = 1;
}

void I2C1_WAIT(void){
    // Wait until the lower 5 bits of CON(SEN,RSEN,PEN,RCEN,ACKEN) as 0
    // AND wait until TRSTAT is 0 (Transmit not in progress)
    while((I2C1CONbits.SEN || I2C1CONbits.RSEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN)||(I2C1STATbits.TRSTAT));
   
}

void I2C1_START(void){
    I2C1_WAIT(); // Ensure bus is IDLE
    I2C1CONbits.SEN = 1;
}

void I2C1_STOP(void){
    I2C1_WAIT(); // Ensure bus is IDLE
    I2C1CONbits.PEN = 1;
}

//return "0" if Slave ACKED, "1" if slave NACKED
uint8_t I2C1_WRITE(uint8_t DATA){
    
    I2C1_WAIT();
    I2C1TRN = DATA;
    I2C1_WAIT();
    return I2C1STATbits.ACKSTAT; //0 = success, 1 = Fail
}


