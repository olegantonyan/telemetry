#include "usbd_cdc_if.h"
#include "cmsis_os.h"
#include <stdarg.h>
#include "syslog/syslog.h"

#define MINIM(a ,b) ((a) < (b) ? (a) : (b))

static void init();
static void thread(void const *arg);
osThreadId thread_handle;
static bool initialized = false;

void syslog_write(const char *fmt, ...) {
  if (!initialized) {
    init();
  }

  //CDC_Transmit_FS((uint8_t *)str, strlen(str));

  va_list args;
	va_start(args, fmt);
  char buf[128] = { 0 };
  memcpy(buf, fmt, sizeof(buf) - 1);
  buf[MINIM(strlen(fmt), sizeof(buf) - 1)] = '\n';
  vprintf(buf, args);
  va_end(args);
}

static void thread(void const *arg) {
  while(true) {
    osDelay(500);

    //char voltage_string[16] = {0};
    //adc_voltage_formatted_string(voltage_string, sizeof voltage_string);
    //char result_string[64] = {0};
    //printf("voltage: %s\n", voltage_string);
    //snprintf(result_string, sizeof result_string, "voltage: %s\n", voltage_string);
    //CDC_Transmit_FS((uint8_t *)result_string, strlen(result_string));
  }
}

static void init() {
  osThreadDef(syslog_thread, thread, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(syslog_thread), NULL);
  initialized = true;
}
