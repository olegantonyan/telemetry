#include <stdio.h>

#include "core/core.h"
#include "rf/rf.h"
#include "cmsis_os.h"
#include "main.h"
#include "serial_log/serial_log.h"
#include "leds/leds.h"
#include "gui/gui.h"

static void thread(void const *arg);
static osThreadId thread_handle;

void core_init() {
  osThreadDef(core_thread, thread, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(core_thread), NULL);
}

static void thread(void const *arg) {
  serial_log_write("core start\n");
  leds_status_off();
  while(true) {

    uint8_t buf[64] = { 0 };
    if (rf_receive(buf, 500)) {
      leds_status_flash(200);

      uint8_t voltage_integer = buf[1];
      uint16_t voltage_fractional = (buf[2] << 8) | buf[3];

      gui_display_voltage(voltage_integer, voltage_fractional);

      char string[16] = { 0 };
      snprintf(string, sizeof string, "v: %u.%04u\n", voltage_integer, voltage_fractional);
      serial_log_write(string);
    } else {
      gui_display_voltage(0, 0);
    }
  }
}
