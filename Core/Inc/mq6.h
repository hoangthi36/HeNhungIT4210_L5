#ifndef MQ6_H
#define MQ6_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint16_t adc_raw;
    uint16_t adc_voltage_mv;
    uint16_t sensor_voltage_mv;

    float rs_ohm;
    float ratio;
    float ppm;

    bool valid;
} MQ6_Result_t;

void MQ6_Process(uint16_t adc_raw, MQ6_Result_t *result);

#endif
