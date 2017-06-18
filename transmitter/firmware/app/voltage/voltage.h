#ifndef __VOLTAGE_H
#define __VOLTAGE_H

#include "stm32f1xx_hal.h"

typedef struct {
  uint16_t integer;
  uint16_t fractional;
} Voltage;

void voltage_init(ADC_HandleTypeDef *adc_handle);
Voltage voltage_read();
void voltage_formatted_string(char * output_string);

#endif
