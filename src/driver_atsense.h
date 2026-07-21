/**
 * @file driver_atsense.h
 * @brief High-speed metrology sampling.
 */
#ifndef DRIVER_ATSENSE_H
#define DRIVER_ATSENSE_H

#include <stdint.h>

typedef struct {
    int32_t v_a;
    int32_t v_b;
    int32_t v_c;
    int32_t i_a;
    int32_t i_b;
    int32_t i_c;
    int32_t i_n;
} MetrologyData_t;

extern MetrologyData_t meter_data;
extern volatile uint8_t new_data_ready;

void ATSENSE_Init(void);
void ATSENSE_FetchData(void);

#endif