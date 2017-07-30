#include <stdio.h>

#include "gui/gui.h"
#include "cmsis_os.h"
#include "sh1106.h"
#include "stm32f1xx_hal.h"

static void thread(void const *arg);
static osThreadId thread_handle;

static volatile uint8_t voltage_integer = 0;
static volatile uint16_t voltage_fractional = 0;

static void wdt_kick();

extern I2C_HandleTypeDef hi2c1;
extern IWDG_HandleTypeDef hiwdg;

void gui_init() {
  osThreadDef(gui_thread, thread, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(gui_thread), NULL);
}

void gui_display_voltage(uint8_t integer, uint16_t fractional) {
  voltage_integer = integer;
  voltage_fractional = fractional;
}

static void thread(void const *arg) {
  sh1106_init(&hi2c1, &wdt_kick);

  while(true) {
    char string[6] = { 0 };
    snprintf(string, sizeof string, "%02u.%02u", voltage_integer, voltage_fractional);
    sh1106_draw_string(string, 0, 0);

    sh1106_render();
    osDelay(200);
  }
}

static void wdt_kick() {
  HAL_IWDG_Refresh(&hiwdg);
}
