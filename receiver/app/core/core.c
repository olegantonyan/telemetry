#include "stm32f1xx_hal.h"
#include "core/core.h"
#include "rf/rf.h"
#include "cmsis_os.h"

static void thread(void const *arg);
static osThreadId thread_handle;

void core_init() {
  osThreadDef(core_thread, thread, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(core_thread), NULL);
}

static void thread(void const *arg) {
  while(true) {


    osDelay(400);
  }
}
