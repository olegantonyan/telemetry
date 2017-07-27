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

  sh1106_draw_character('4', 0, 0);
  sh1106_draw_character('5', 26, 0);
  sh1106_draw_character('6', 52, 0);
  sh1106_draw_character('7', 52, 0);

  //sh1106_draw_rectangle( 4, 4, 30, 0);

  /*sh1106_draw_character(0xff, 26, 0);

  sh1106_draw_character(0xff, 52, 0);

  sh1106_draw_character(0xff, 78, 0);

  sh1106_draw_character(0xff, 104, 0);*/

  sh1106_draw_character('0', 0, 33);

  sh1106_draw_character('1', 26, 33);
  sh1106_draw_character('2', 52, 33);
  sh1106_draw_character('3', 78, 33);


  //uint16_t i = 0;
  //uint16_t state = 1;
  while(true) {
    //sh1106_set_pixel(i, 0, state);
    sh1106_render();

    /*if(i++ >= 127) {
      i = 0;
      if (state == 1) {
        state = 0;
      } else {
        state = 1;
      }

    }*/

    osDelay(30);
  }
}

static void wdt_kick() {
  HAL_IWDG_Refresh(&hiwdg);
}
