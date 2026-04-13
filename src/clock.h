#ifndef CLOCK_H
#define	CLOCK_H

/**
  Section: Included Files
*/

#include <stdbool.h>

#ifndef _XTAL_FREQ
#define _XTAL_FREQ  32000000UL
#endif

#define CLOCK_SystemFrequencyGet()        (32000000UL)

#define CLOCK_PeripheralFrequencyGet()    (CLOCK_SystemFrequencyGet() / 2)

#define CLOCK_InstructionFrequencyGet()   (CLOCK_SystemFrequencyGet() / 2)

void CLOCK_Initialize(void);


#endif	/* CLOCK_H */

/**
 End of File
*/