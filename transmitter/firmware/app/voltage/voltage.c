#include <math.h>

#include "voltage/voltage.h"

#define VREF 3.3
#define DIVIDER_COEFFICIENT 7.96
#define VOLTS_PER_BIT_AFTER_DIVIDER (VREF / 4096)
#define VOLTS_PER_BIT_BEFORE_DIVIDER (VOLTS_PER_BIT_AFTER_DIVIDER * DIVIDER_COEFFICIENT)

static volatile uint16_t buffer[16] = {0};
static const size_t buffer_elements = sizeof buffer / sizeof buffer[0];

static double average_voltage();

void voltage_init(ADC_HandleTypeDef *adc_handle) {
  HAL_ADCEx_Calibration_Start(adc_handle);
  HAL_ADC_Start_DMA(adc_handle, (uint32_t *)buffer, buffer_elements);
}

Voltage voltage_read() {
  double val = average_voltage() * VOLTS_PER_BIT_BEFORE_DIVIDER;

  Voltage result;
  result.integer = (uint16_t)val;
  result.fractional = trunc((val - result.integer) * 10000);
  return result;
}

void voltage_formatted_string(char * output_string) {
  Voltage v = voltage_read();
  sprintf(output_string, "%u.%04u\n", v.integer, v.fractional);
}

static double average_voltage() {
  double value = 0;
  for (size_t i = 0; i < buffer_elements; i++) {
    value += buffer[i];
  }
  return value / buffer_elements;
}
