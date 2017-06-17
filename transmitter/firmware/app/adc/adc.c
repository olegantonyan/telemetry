#include "adc/adc.h"

static uint32_t adc_buf[16] = {0};

void adc_start(ADC_HandleTypeDef *adc_handle) {
  HAL_ADCEx_Calibration_Start(adc_handle);
  HAL_ADC_Start_DMA(adc_handle, adc_buf, 16);
}

uint32_t *adc_lipo_voltage() {
  return adc_buf;
}
