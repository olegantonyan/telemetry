#include "buzzer.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

extern TIM_HandleTypeDef htim3;

static void pwm_start();
static void pwm_stop();
static void tone_timeout(const void * args);
static void total_timeout(const void * args);

osTimerDef(tone_timer, tone_timeout);
osTimerId tone_timer_handle = NULL;
osTimerDef(total_timer, total_timeout);
osTimerId total_timer_handle = NULL;

static bool tone_state = false;

void buzzer_short_beeps_start(uint32_t duration) {
  if (tone_timer_handle || total_timer_handle) {
    return;
  }

  tone_timer_handle = osTimerCreate(osTimer(tone_timer), osTimerPeriodic, NULL);
  osTimerStart(tone_timer_handle, 300);

  total_timer_handle = osTimerCreate(osTimer(total_timer), osTimerOnce, NULL);
  osTimerStart(total_timer_handle, duration);
}

static void pwm_start() {
  HAL_TIM_Base_Start(&htim3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

static void pwm_stop() {
  HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
  HAL_TIM_Base_Stop(&htim3);
}

static void tone_timeout(const void * args) {
  if (tone_state) {
    pwm_stop();
    tone_state = false;
  } else {
    pwm_start();
    tone_state = true;
  }
}

static void total_timeout(const void * args) {
  osTimerStop(tone_timer_handle);
  osTimerDelete(tone_timer_handle);
  tone_timer_handle = NULL;
  osTimerDelete(total_timer_handle);
  total_timer_handle = NULL;
  pwm_stop();
  tone_state = false;
}
