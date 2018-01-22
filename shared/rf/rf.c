#include <string.h>

#include "cc1101/cc1101.h"
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

static void cc1101_spi_write_read(const uint8_t *tx_data, uint8_t *rx_data, uint16_t length);
static void cc1101_chip_select(bool state);
static void cc1101_delay(uint32_t ms);
static void cc1101_wait_chip_ready();

void rf_init() {
  CC1101_t config;
  config.write_read = cc1101_spi_write_read;
  config.chip_select = cc1101_chip_select;
  config.delay = cc1101_delay;
  config.wait_chip_ready = cc1101_wait_chip_ready;
  cc1101_init(&config);

  //HAL_NVIC_DisableIRQ(EXTI0_IRQn);

  //HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  tx_semaphore = osSemaphoreCreate(osSemaphore(tx_semaphore), 1);
  rx_queue = osMessageCreate(osMessageQ(rx_queue), NULL);
  rx_buffer_mutex = osMutexCreate(osMutex(rx_buffer_mutex));
}

void rf_transmit(const uint8_t *data) {
  //osSemaphoreWait(tx_semaphore, osWaitForever);
  cc1101_transmit(data, RF_PACKET_LENGTH);
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
  printf("INTERRUPT!\n");

  //SI4463_GetInterrupts(&si4463);

  /*
  static uint8_t buf[RF_PACKET_LENGTH] = { 0 };
  osMutexWait(rx_buffer_mutex, osWaitForever);
  memset(buf, 0, sizeof buf);
  SI4463_ReadRxFifo(&si4463, buf, RF_PACKET_LENGTH);
  osMutexRelease(rx_buffer_mutex);
  osMessagePut(rx_queue, (uint32_t)buf, 0);
  */
}

static void cc1101_spi_write_read(const uint8_t *tx_data, uint8_t *rx_data, uint16_t length) {
  if (!rx_data) {
    HAL_SPI_Transmit(&hspi1, (uint8_t *)tx_data, length, 100);
  } else {
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)tx_data, (uint8_t *)rx_data, length, 100);
  }
}

static void cc1101_chip_select(bool state) {
  HAL_GPIO_WritePin(CC1101_CSN_GPIO_Port, CC1101_CSN_Pin, state ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void cc1101_delay(uint32_t ms) {
  //osDelay(ms);
  HAL_Delay(ms);
  //for(volatile uint32_t i = 0; i < 1000; i++);
}

static void cc1101_wait_chip_ready() {
  while(HAL_GPIO_ReadPin(CC1101_GDO2_GPIO_Port, CC1101_GDO2_Pin) == GPIO_PIN_SET);
}
