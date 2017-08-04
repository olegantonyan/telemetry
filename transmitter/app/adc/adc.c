#include <math.h>

#include "adc/adc.h"

#define VREF 3.3

#define DIVIDER_COEFFICIENT_VOLTAGE 7.96
#define VOLTS_PER_BIT (VREF / 4096)
#define VOLTS_PER_BIT_INPUT_VOLTAGE (VOLTS_PER_BIT * DIVIDER_COEFFICIENT_VOLTAGE)

static volatile uint16_t buffer[16] = {0};
static const size_t buffer_elements = sizeof buffer / sizeof buffer[0];

static double average_voltage();

void adc_init(ADC_HandleTypeDef *adc_handle) {
  HAL_ADCEx_Calibration_Start(adc_handle);
  HAL_ADC_Start_DMA(adc_handle, (uint32_t *)buffer, buffer_elements);
}

ADC_Current adc_current_read() {
  ADC_Current result;

  return result;
}

ADC_Voltage adc_voltage_read() {
  double val = average_voltage() * VOLTS_PER_BIT_INPUT_VOLTAGE;

  ADC_Voltage result;
  result.integer = (uint16_t)val;
  result.fractional = trunc((val - result.integer) * 10000);
  return result;
}

void adc_voltage_formatted_string(char * output_string, size_t max_length) {
  ADC_Voltage v = adc_voltage_read();
  snprintf(output_string, max_length, "%u.%04u", v.integer, v.fractional);
}

static double average_voltage() {
  double value = 0;
  for (size_t i = 0; i < buffer_elements; i++) {
    value += buffer[i];
  }
  return value / buffer_elements;
}
