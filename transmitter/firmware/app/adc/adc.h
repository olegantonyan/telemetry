#ifndef __ADC_H
#define __ADC_H

#include "stm32f1xx_hal.h"

void adc_start(ADC_HandleTypeDef *adc_handle);
void adc_lipo_voltage(char * output_string);
uint16_t *get_buf();

#endif
