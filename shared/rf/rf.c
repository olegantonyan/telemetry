#include <string.h>

#include "stm32f1xx_hal.h"
#include "rf/rf.h"
#include "cmsis_os.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

static osSemaphoreId tx_semaphore;
osSemaphoreDef(tx_semaphore);
static osMessageQId rx_queue;
osMessageQDef(rx_queue, 1, uint8_t *);
static osMutexId rx_buffer_mutex;
osMutexDef(rx_buffer_mutex);

void rf_init() {

  //HAL_NVIC_DisableIRQ(EXTI0_IRQn);

  //HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  tx_semaphore = osSemaphoreCreate(osSemaphore(tx_semaphore), 1);
  rx_queue = osMessageCreate(osMessageQ(rx_queue), NULL);
  rx_buffer_mutex = osMutexCreate(osMutex(rx_buffer_mutex));
}

void rf_transmit(const uint8_t *data) {
  osSemaphoreWait(tx_semaphore, osWaitForever);
}

bool rf_receive(uint8_t * data, uint32_t timeout_ms) {
  if (timeout_ms == 0) {
    timeout_ms = osWaitForever;
  }
  osEvent evt = osMessageGet(rx_queue, timeout_ms);
  if (evt.status == osEventMessage) {
    osMutexWait(rx_buffer_mutex, osWaitForever);
    memcpy(data, evt.value.p, RF_PACKET_LENGTH);
    osMutexRelease(rx_buffer_mutex);
    return true;
  }
  return false;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  SI4463_GetInterrupts(&si4463);

  /*
  static uint8_t buf[RF_PACKET_LENGTH] = { 0 };
  osMutexWait(rx_buffer_mutex, osWaitForever);
  memset(buf, 0, sizeof buf);
  SI4463_ReadRxFifo(&si4463, buf, RF_PACKET_LENGTH);
  osMutexRelease(rx_buffer_mutex);
  osMessagePut(rx_queue, (uint32_t)buf, 0);
  */


  }
}
