#include "usbd_cdc_if.h"
#include "cmsis_os.h"

#include "rf/rf.h"

static void thread(void const *arg);
osThreadId thread_handle;

void serial_log_init() {
  osThreadDef(serial_log_thread, thread, osPriorityNormal, 0, 128);
  thread_handle = osThreadCreate(osThread(serial_log_thread), NULL);
}

static void thread(void const *arg) {
  while(true) {
    uint8_t buf[64] = { 0 };
    if (rf_receive(buf)) {
      CDC_Transmit_FS(buf, 7);
    }

    //const char *result_string = "hello\n";
    //CDC_Transmit_FS((uint8_t *)result_string, strlen(result_string));
    //osDelay(500);
  }
}
