#ifndef SENSOR_CONVERSION_H
#define SENSOR_CONVERSION_H

#include <stdint.h>

uint32_t adc_raw_to_millivolts(uint32_t raw_count,
                               uint32_t reference_mv);

int32_t tmp36_millivolts_to_tenths_c(uint32_t voltage_mv);

#endif /* SENSOR_CONVERSION_H */
