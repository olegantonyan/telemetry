#include "usbd_cdc_if.h"
#include "cmsis_os.h"

#include "serial_log/serial_log.h"
#include "adc/adc.h"

static void thread(void const *arg);
osThreadId thread_handle;

void serial_log_init() {
  osThreadDef(serial_log_thread, thread, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(serial_log_thread), NULL);
}

void serial_log_write(const char *str) {

}

static void thread(void const *arg) {
  CDC_Transmit_FS((uint8_t *)"started", strlen("started"));
  while(1) {
    char voltage_string[16] = {0};
    adc_voltage_formatted_string(voltage_string, sizeof voltage_string);
    char result_string[64] = {0};
    snprintf(result_string, sizeof result_string, "voltage: %s\n", voltage_string);
    CDC_Transmit_FS((uint8_t *)result_string, strlen(result_string));

    osDelay(500);
  }
}
