#ifndef __ADC_H
#define __ADC_H

#include "stm32f1xx_hal.h"

typedef struct {
  uint16_t integer;
  uint16_t fractional;
} ADC_Voltage;

typedef struct {
  uint16_t integer;
  uint16_t fractional;
} ADC_Current;

void adc_init(ADC_HandleTypeDef *adc_handle);
ADC_Voltage adc_voltage_read();
ADC_Current adc_current_read();
void adc_voltage_formatted_string(char * output_string, size_t max_length);

#endif
