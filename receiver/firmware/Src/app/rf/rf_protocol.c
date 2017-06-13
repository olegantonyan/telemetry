#include "cmsis_os.h"
#include "rf/rf_protocol.h"

static void thread(void const *arg);

static volatile uint8_t buffer[128] = { 0 };
static osThreadId rf_thread_handle = NULL;
static osMessageQId queue = NULL;

static const uint8_t preamble[] = { 0xAA, 0x55, 0xAA, 0x55, 0x00, 0x00, 0x55, 0xAA, 0x00 };
static const uint8_t postamble[] = { 0x00, 0x7E, 0x7E };

uint8_t rf_init() {
  //osMessageQDef_t queue_def = {  };
  //queue = osMessageCreate(NULL, 1);
  //osThreadDef(rf_thread, thread, osPriorityNormal, 0, 128);
  //rf_thread_handle = osThreadCreate(osThread(rf_thread), NULL);
}

uint8_t on_rf_phy_byte_received(uint8_t byte) {
  for (size_t i = sizeof(buffer) - 1u; i > 0; i--) {
    buffer[i] = buffer[i - 1u];
  }
  buffer[0] = byte;

  if (buffer[0] == 0x7E && buffer[1] == 0x7E && buffer[2] == 0x00) {
    //osSemaphoreRelease(sema);
    return 1u;
  }
  return 0;
}

static void thread(void const *arg) {
  while (1) {
    //osSemaphoreWait(sema, osWaitForever);
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
  }
}
