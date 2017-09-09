#include "core/core.h"
#include "stm32f1xx_hal.h"
#include "adc/adc.h"
#include "rf/rf.h"
#include "cmsis_os.h"
#include "main.h"
#include "syslog/syslog.h"

static uint8_t device_id();
static void thread(void const *arg);
static osThreadId thread_handle;

void core_init() {
  osThreadDef(core_thread, thread, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(core_thread), NULL);
}

static void thread(void const *arg) {
  syslog_write("started");
  while(true) {
    uint8_t data[RF_PACKET_LENGTH] = { 0x00 };

    ADC_Voltage volts = adc_voltage_read();
    ADC_Current amps = adc_current_read();
    data[0] = device_id();
    data[1] = (uint8_t)(volts.integer & 0xFF);
    data[2] = (uint8_t)((volts.fractional >> 8) & 0xFF);
    data[3] = (uint8_t)(volts.fractional & 0xFF);
    data[4] = (uint8_t)(amps.integer & 0xFF);
    data[5] = (uint8_t)((amps.fractional >> 8) & 0xFF);
    data[6] = (uint8_t)(amps.fractional & 0xFF);
    data[7] = 0;
    data[8] = 0;
    data[9] = 0;
    data[10] = 0;
    data[11] = 0;

    HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
    rf_transmit(data);
    HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);

    osDelay(20);
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
