#include "core/core.h"
#include "stm32f1xx_hal.h"
#include "adc/adc.h"
#include "rf/rf.h"
#include "cmsis_os.h"

static uint8_t device_id();
static void thread(void const *arg);
static osThreadId thread_handle;

void core_init() {
  osThreadDef(core_thread, thread, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(core_thread), NULL);
}

static void thread(void const *arg) {
  while(1) {
    uint8_t data[7] = { 0 };

    ADC_Voltage volts = adc_voltage_read();
    data[0] = device_id();
    data[1] = (uint8_t)(volts.integer & 0xFF);
    data[2] = (uint8_t)((volts.fractional >> 8) & 0xFF);
    data[3] = (uint8_t)(volts.fractional & 0xFF);

    rf_transmit(data);

    osDelay(400);
  }
}

static uint8_t device_id() {
  uint8_t result = 0;
  uint8_t data[12] = { 0 };
  for(size_t i = 0; i < sizeof data; i++) {
    result += *(uint8_t *)(UID_BASE + i);
  }
  return result;
}
