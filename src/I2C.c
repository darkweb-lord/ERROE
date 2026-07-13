#define FCY 16000000UL      /* must come BEFORE libpic30.h */
#include <xc.h>         // This loads the p24FV32KA304.h definitions automatically
#include <libpic30.h>       /* provides __delay_us / __delay_ms */
#include "I2c_Header.h"

void I2C1_BUS_RECOVERY(void)
{
    I2C1CONbits.I2CEN = 0;        // release pins to GPIO

    ODCB |= (1 << 5) | (1 << 6);  // RB5 (SDA) + RB6 (SCL) open-drain
    TRISB |=  (1 << 5);           // SDA = input (read it)
    TRISB &= ~(1 << 6);           // SCL = output
    LATB  |=  (1 << 6);           // SCL high

    for (int i = 0; i < 9; i++) {
        if (PORTBbits.RB5) break;         // SDA released -> done
        LATB &= ~(1 << 6); __delay_us(5); // SCL low
        LATB |=  (1 << 6); __delay_us(5); // SCL high
    }

    // STOP condition: SDA low->high while SCL high
    TRISB &= ~(1 << 5); LATB &= ~(1 << 5); __delay_us(5);  // SDA output low
    LATB |= (1 << 6); __delay_us(5);                        // SCL high
    LATB |= (1 << 5); __delay_us(5);                        // SDA high = STOP

    TRISB |= (1 << 5) | (1 << 6);  // release both back to inputs
}

void I2C_INIT(void){
    I2C1CON = 0x0000;
    I2C1STAT =0x0000;
    
    I2C1BRG = 157 ; // 157 SCL operate at 100KHz on 16MHz 
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

void I2C1_RESTART(void){
    I2C1_WAIT();
    I2C1CONbits.RSEN = 1;
}

//return "0" if Slave ACKED, "1" if slave NACKED
uint8_t I2C1_WRITE(uint8_t DATA){
    I2C1_WAIT();
    I2C1TRN = DATA;
    I2C1_WAIT();
    return I2C1STATbits.ACKSTAT; // 0 = success, 1 = Fail
}

uint8_t I2C1_READ(uint8_t ack){
    uint8_t data;
    I2C1_WAIT();
    I2C1CONbits.RCEN = 1;       // Enable Receive mode
    
    while(!I2C1STATbits.RBF);    // Wait for buffer to fill
    data = I2C1RCV;              // Read data
    
    I2C1_WAIT();
    I2C1CONbits.ACKDT = ack;    // 0 = ACK, 1 = NACK
    I2C1CONbits.ACKEN = 1;      // Send ACK/NACK
    return data;
}