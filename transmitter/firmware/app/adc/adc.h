#ifndef __ADC_H
#define __ADC_H

#include "stm32f1xx_hal.h"

void adc_start(ADC_HandleTypeDef *adc_handle);
uint32_t *adc_lipo_voltage();

#endif
