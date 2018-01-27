#include <string.h>

#include "cc1101/cc1101.h"
#include "stm32f1xx_hal.h"
#include "rf/rf.h"
#include "cmsis_os.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

typedef struct {
  uint8_t data[RF_PACKET_LENGTH];
} RxMessage_t;

osPoolDef(mpool, 16, RxMessage_t);
static osPoolId mpool;

osSemaphoreDef(tx_semaphore);
static osSemaphoreId tx_semaphore;

osMessageQDef(rx_queue, 16, RxMessage_t);
static osMessageQId rx_queue;

static void cc1101_spi_write_read(const uint8_t *tx_data, uint8_t *rx_data, uint16_t length);
static void cc1101_chip_select(bool state);
static void cc1101_wait_chip_ready();
static void cc1101_packet_received(const uint8_t *data, uint16_t length);
static void cc1101_packet_sent();

void rf_init() {
  HAL_NVIC_DisableIRQ(EXTI1_IRQn);

  tx_semaphore = osSemaphoreCreate(osSemaphore(tx_semaphore), 1);
  rx_queue = osMessageCreate(osMessageQ(rx_queue), NULL);
  mpool = osPoolCreate(osPool(mpool));

  CC1101_t config;
  config.write_read = cc1101_spi_write_read;
  config.chip_select = cc1101_chip_select;
  config.wait_chip_ready = cc1101_wait_chip_ready;
  config.packet_received = cc1101_packet_received;
  config.packet_sent = cc1101_packet_sent;
  cc1101_init(&config);

  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

void rf_transmit(const uint8_t *data) {
  osSemaphoreWait(tx_semaphore, osWaitForever);
  cc1101_transmit(data, RF_PACKET_LENGTH);
}

bool rf_receive(uint8_t *data, uint32_t timeout_ms) {
  if (timeout_ms == 0) {
    timeout_ms = osWaitForever;
  }
  osEvent evt = osMessageGet(rx_queue, timeout_ms);
  if (evt.status == osEventMessage) {
    RxMessage_t *message = (RxMessage_t *)evt.value.p;
    memcpy(data, message->data, RF_PACKET_LENGTH);
    osPoolFree(mpool, message);
    return true;
  }
  return false;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  UNUSED(GPIO_Pin);
  cc1101_gdo_interrupt();
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

static void cc1101_wait_chip_ready() {
  while(HAL_GPIO_ReadPin(CC1101_GDO2_GPIO_Port, CC1101_GDO2_Pin) == GPIO_PIN_SET);
}

static void cc1101_packet_received(const uint8_t *data, uint16_t length) {
  RxMessage_t *message = (RxMessage_t *)osPoolAlloc(mpool);
  memcpy(message->data, data + 1, RF_PACKET_LENGTH);
  osMessagePut(rx_queue, (uint32_t)message, osWaitForever);
}

static void cc1101_packet_sent() {
  osSemaphoreRelease(tx_semaphore);
}
