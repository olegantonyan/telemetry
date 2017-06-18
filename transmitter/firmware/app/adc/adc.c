#include "adc/adc.h"

static uint16_t adc_buf[16] = {0};

void adc_start(ADC_HandleTypeDef *adc_handle) {
  HAL_ADCEx_Calibration_Start(adc_handle);
  HAL_ADC_Start_DMA(adc_handle, adc_buf, sizeof adc_buf / sizeof adc_buf[0]);
}

uint16_t *get_buf() {
  return adc_buf;
}

void adc_lipo_voltage(char * output_string) {
  double value = 0;
  size_t buf_elements = sizeof adc_buf / sizeof adc_buf[0];
  for (size_t i = 0; i < buf_elements; i++) {
    value += adc_buf[i];
  }
  value = value / buf_elements;

  // 7.96 voltage divider
  // 0.0008056640625 per bit
  double val = value * 0.0008056640625 * 7.96;
  uint16_t int_part = (uint16_t)val;
  uint16_t dec_part = trunc((val - int_part) * 10000);

  sprintf(output_string, "%u.%04u\n", int_part, dec_part);
}
