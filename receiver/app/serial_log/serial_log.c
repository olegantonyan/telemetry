#include <stdio.h>

#include "usbd_cdc_if.h"
#include "cmsis_os.h"

#include "rf/rf.h"

static void thread(void const *arg);
osThreadId thread_handle;

void serial_log_init() {
  osThreadDef(serial_log_thread, thread, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(serial_log_thread), NULL);
}

void serial_log_write(const char *str) {
  //CDC_Transmit_FS((uint8_t *)str, strlen(str));
  printf(str);
}

static void thread(void const *arg) {
  while(true) {

    //const char *result_string = "hello\n";
    //CDC_Transmit_FS((uint8_t *)result_string, strlen(result_string));
    osDelay(500);
  }
}
