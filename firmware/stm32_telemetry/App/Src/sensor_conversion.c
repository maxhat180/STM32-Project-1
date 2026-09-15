#include "sensor_conversion.h"

uint32_t adc_raw_to_millivolts(uint32_t raw_count, uint32_t reference_mv){
    return (raw_count * reference_mv) / 4095U;
}

int32_t tmp36_millivolts_to_tenths_c(uint32_t voltage_mv){
    return (int32_t)voltage_mv - 500;
}
