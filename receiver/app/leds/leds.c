#include "leds/leds.h"
#include "main.h"
#include "cmsis_os.h"

static void timeout(void const *);

osTimerDef(leds_timer, timeout);
osTimerId tmr = NULL;

void leds_status_flash(uint32_t ms) {
  if (tmr) {
    osTimerStop(tmr);
  } else {
    leds_status_on();
    tmr = osTimerCreate(osTimer(leds_timer), osTimerOnce, NULL);
  }
  osTimerStart(tmr, ms);
}

void leds_status_on() {
  HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
}

void leds_status_off() {
  HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
}

static void timeout(void const *arg) {
  UNUSED(arg);
  leds_status_off();
  osTimerDelete(tmr);
  tmr = NULL;
}
