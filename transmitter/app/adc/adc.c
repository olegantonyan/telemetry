#include <math.h>

#include "adc/adc.h"

#define VREF 3.3
#define VOLTS_PER_BIT (VREF / 4096)

#define DIVIDER_COEFFICIENT_VOLTAGE 7.96
#define VOLTS_PER_BIT_INPUT_VOLTAGE (VOLTS_PER_BIT * DIVIDER_COEFFICIENT_VOLTAGE)

#define VOLT_PER_AMP_CURRENT 0.01 // 10 mV for ACS758-200B
#define AMPS_PER_BIT (VOLTS_PER_BIT / VOLT_PER_AMP_CURRENT)

static volatile uint16_t buffer[16] = {0};
static const size_t buffer_elements = sizeof buffer / sizeof buffer[0];

static double average_voltage();
static double average_current();

void adc_init(ADC_HandleTypeDef *adc_handle) {
  HAL_ADCEx_Calibration_Start(adc_handle);
  HAL_ADC_Start_DMA(adc_handle, (uint32_t *)buffer, buffer_elements);
}

ADC_Current adc_current_read() {
  double val = (2048 - average_current()) * AMPS_PER_BIT; // bi-directional - shift 0

  ADC_Current result;
  result.integer = (uint16_t)val;
  result.fractional = trunc((val - result.integer) * 10000);
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
  for (size_t i = 0; i < buffer_elements; i += 2) {
    value += buffer[i];
  }
  return value / buffer_elements * 2;
}

static double average_current() {
  double value = 0;
  for (size_t i = 1; i < buffer_elements; i += 2) {
    value += buffer[i];
  }
  return value / buffer_elements * 2;
}
