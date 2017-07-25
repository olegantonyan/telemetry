#include "gui/gui.h"
#include "cmsis_os.h"
#include "sh1106.h"
#include "stm32f1xx_hal.h"

static void thread(void const *arg);
static osThreadId thread_handle;

extern I2C_HandleTypeDef hi2c1;

void gui_init() {
  osThreadDef(gui_thread, thread, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(gui_thread), NULL);
}

static void thread(void const *arg) {
  sh1106_init(&hi2c1);

  int i = 0;
  int j = 0;
  while(true) {
    sh1106_set_pixel(i, j, 1);
    sh1106_render();

    if(i++ > 63) {
      i = 0;
    }
    if(j++ > 127) {
      j = 0;
    }

    osDelay(30);
  }
}
