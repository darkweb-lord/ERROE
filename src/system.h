/**
 * @file system.h
 * @brief System-level definitions and clock configuration.
 */
#ifndef SYSTEM_H
#define	SYSTEM_H

// MUST define FCY before libpic30.h so delay functions calculate correctly
#define FCY 16000000UL 

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Initializes the PIC24 system clock.
 */
void SYSTEM_Initialize(void);

#endif	/* SYSTEM_H */