#include "gui/gui.h"
#include "cmsis_os.h"
#include "sh1106.h"
#include "stm32f1xx_hal.h"

static void thread(void const *arg);
static osThreadId thread_handle;

static void wdt_kick();

extern I2C_HandleTypeDef hi2c1;
extern IWDG_HandleTypeDef hiwdg;

void gui_init() {
  osThreadDef(gui_thread, thread, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(gui_thread), NULL);
}

static void thread(void const *arg) {
  sh1106_init(&hi2c1, &wdt_kick);

  uint16_t i = 0;
  uint16_t state = 1;
  while(true) {
    sh1106_set_pixel(i, 0, state);
    sh1106_render();

    if(i++ >= 127) {
      i = 0;
      if (state == 1) {
        state = 0;
      } else {
        state = 1;
      }

    }

    osDelay(30);
  }
}

static void wdt_kick() {
  HAL_IWDG_Refresh(&hiwdg);
}