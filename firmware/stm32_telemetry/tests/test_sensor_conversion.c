#include <assert.h>

#include "sensor_conversion.h"

int main(void)
{
    assert(adc_raw_to_millivolts(0U, 3300U) == 0U);
    assert(adc_raw_to_millivolts(4095U, 3300U) == 3300U);
    assert(adc_raw_to_millivolts(2048U, 3300U) == 1650U);

    assert(tmp36_millivolts_to_tenths_c(500U) == 0);
    assert(tmp36_millivolts_to_tenths_c(750U) == 250);
    assert(tmp36_millivolts_to_tenths_c(400U) == -100);

    return 0;
}