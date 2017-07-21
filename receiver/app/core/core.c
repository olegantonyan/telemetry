#include <stdio.h>

#include "core/core.h"
#include "rf/rf.h"
#include "cmsis_os.h"
#include "main.h"
#include "serial_log/serial_log.h"
#include "leds/leds.h"

static void thread(void const *arg);
static osThreadId thread_handle;

void core_init() {
  osThreadDef(core_thread, thread, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(core_thread), NULL);
}

static void thread(void const *arg) {
  serial_log_write("core start");
  while(true) {

    uint8_t buf[64] = { 0 };
    if (rf_receive(buf)) {
      leds_status_flash(200);

      uint8_t voltage_integer = buf[1];
      uint16_t voltage_fractional = (buf[2] << 8) | buf[3];

      char string[16] = { 0 };
      snprintf(string, sizeof string, "v: %u.%04u\n", voltage_integer, voltage_fractional);
      serial_log_write(string);
    }
  }
}
