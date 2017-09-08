#include <stdio.h>

#include "core/core.h"
#include "rf/rf.h"
#include "cmsis_os.h"
#include "main.h"
#include "log/log.h"
#include "leds/leds.h"
#include "gui/gui.h"
#include "buzzer/buzzer.h"

static void thread(void const *arg);
static osThreadId thread_handle;

void core_init() {
  osThreadDef(core_thread, thread, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(core_thread), NULL);
}

static void thread(void const *arg) {
  log_write("core start\n");
  leds_status_off();

  while(true) {
    uint8_t buf[RF_PACKET_LENGTH] = { 0 };

    if (rf_receive(buf, 300)) {
      uint8_t voltage_integer = buf[1];
      uint16_t voltage_fractional = (buf[2] << 8) | buf[3];
      if (voltage_integer > 99 || voltage_fractional > 9999) {
        continue;
      }
      uint8_t current_integer = buf[4];
      uint16_t current_fractional = (buf[5] << 8) | buf[6];
      if (current_fractional > 9999) {
        //continue;
      }

      leds_status_flash(200);

      gui_display_voltage(voltage_integer, voltage_fractional);
      gui_display_current(current_integer, current_fractional);

      /*char string[26] = { 0 };
      snprintf(string, sizeof string, "v: %u.%04u a: %u.%02u\n", voltage_integer, voltage_fractional, current_integer, current_fractional);
      log_write(string);*/

      if (voltage_integer > 0 && voltage_integer <= 21) {
      //  buzzer_short_beeps_start(5000);
      }

    } else {
      gui_display_voltage(0, 0);
      gui_display_current(0, 0);
    }
  }
}
