#include <string.h>

#include "stm32f1xx_hal.h"
#include "rf/rf.h"
#include "cmsis_os.h"
#include "si4463/si4463.h"
#include "main.h"

#define RF_PACKET_LENGTH (RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH - 4) // 4 bytes for software CRC
#if (RF_PACKET_LENGTH % 4 != 0)
  #error "RF_PACKET_LENGTH must be 4 bytes aligned"
#endif

static si4463_t si4463;
extern SPI_HandleTypeDef hspi1;
extern CRC_HandleTypeDef hcrc;

static osSemaphoreId tx_semaphore;
osSemaphoreDef(tx_semaphore);
static osMessageQId rx_queue;
osMessageQDef(rx_queue, 1, uint8_t *);
static osMutexId rx_buffer_mutex;
osMutexDef(rx_buffer_mutex);

static bool si4463_cts(void);
static void si4463_write_read(uint8_t *tx_data, uint8_t *rx_data, uint16_t length);
static void si4463_set_shutdown(void);
static void si4463_clear_shutdown(void);
static void si4463_select(void);
static void si4463_deselect(void);
static void fake_delay(uint32_t);
static uint32_t crc(const uint8_t *data, size_t length);

void rf_init() {
  si4463.IsCTS = si4463_cts;
  si4463.WriteRead = si4463_write_read;
  si4463.Select = si4463_select;
  si4463.Deselect = si4463_deselect;
  si4463.SetShutdown = si4463_set_shutdown;
  si4463.ClearShutdown = si4463_clear_shutdown;
  si4463.DelayMs = fake_delay;//HAL_Delay;//(void (*)(uint32_t))osDelay;
  /* Disable interrupt pin for init Si4463 */
  HAL_NVIC_DisableIRQ(EXTI0_IRQn);
  /* Init Si4463 with structure */
  SI4463_Init(&si4463);
  /* Clear RX FIFO before starting RX packets */
  SI4463_ClearRxFifo(&si4463);
  /* Start RX mode.
   * SI4463_StartRx() put a chip in non-armed mode in cases:
   * - successfully receive a packet;
   * - invoked RX_TIMEOUT;
   * - invalid receive.
   * For receiveing next packet you have to invoke SI4463_StartRx() again!*/
  SI4463_StartRx(&si4463, false, false, false);
  /* Enable interrupt pin and */
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  /* Clear interrupts after enabling interrupt pin.
   * Without it may be situation when interrupt is asserted but pin not cleared.*/
  SI4463_ClearInterrupts(&si4463);

  tx_semaphore = osSemaphoreCreate(osSemaphore(tx_semaphore), 1);
  rx_queue = osMessageCreate(osMessageQ(rx_queue), NULL);
  rx_buffer_mutex = osMutexCreate(osMutex(rx_buffer_mutex));
}

void rf_transmit(const uint8_t *data) {
  osSemaphoreWait(tx_semaphore, osWaitForever);
  uint8_t buf[RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH];
  memcpy(buf, data, RF_PACKET_LENGTH);

  uint32_t checksum = crc(data, RF_PACKET_LENGTH);
  memcpy(&buf[RF_PACKET_LENGTH], &checksum, sizeof(checksum));

  SI4463_Transmit(&si4463, buf, RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH);
}

bool rf_receive(uint8_t * data) {
  osEvent evt = osMessageGet(rx_queue, osWaitForever);
  if (evt.status == osEventMessage) {
    osMutexWait(rx_buffer_mutex, osWaitForever);
    memcpy(data, evt.value.p, RF_PACKET_LENGTH);
    uint32_t checksum = crc(evt.value.p, RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH);
    osMutexRelease(rx_buffer_mutex);
    return checksum == 0;
  }
  return false;
}

static bool si4463_cts(void) {
	return HAL_GPIO_ReadPin(SI4463_CTS_GPIO_Port, SI4463_CTS_Pin) == GPIO_PIN_SET;
}

static void si4463_write_read(uint8_t *tx_data, uint8_t *rx_data, uint16_t length) {
	HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, length, 100);
}

static void si4463_set_shutdown(void) {
	HAL_GPIO_WritePin(SI4463_SHUTDOWN_GPIO_Port, SI4463_SHUTDOWN_Pin, GPIO_PIN_SET);
}

static void si4463_clear_shutdown(void) {
	HAL_GPIO_WritePin(SI4463_SHUTDOWN_GPIO_Port, SI4463_SHUTDOWN_Pin, GPIO_PIN_RESET);
}

static void si4463_select(void) {
	HAL_GPIO_WritePin(SI4463_nSEL_GPIO_Port, SI4463_nSEL_Pin, GPIO_PIN_RESET);
}

static void si4463_deselect(void) {
	HAL_GPIO_WritePin(SI4463_nSEL_GPIO_Port, SI4463_nSEL_Pin, GPIO_PIN_SET);
}

static void fake_delay(uint32_t ms) {
  UNUSED(ms);
}

static uint32_t crc(const uint8_t *data, size_t length) {
  return HAL_CRC_Calculate(&hcrc, (uint32_t *)data, length / sizeof(uint32_t));
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  SI4463_GetInterrupts(&si4463);

  if (si4463.interrupts.packetSent) {
	  /* Handling this interrupt here */
	  /* Clear TX FIFO */
	  SI4463_ClearTxFifo(&si4463);
	  /* Re-arm StartRX */
	  SI4463_StartRx(&si4463, false, false, false);

    osSemaphoreRelease(tx_semaphore);
  }
  if (si4463.interrupts.packetRx) {
    /* Handling this interrupt here */

    /* Get FIFO data */
    static uint8_t buf[RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH] = { 0 };
    osMutexWait(rx_buffer_mutex, osWaitForever);
    SI4463_ReadRxFifo(&si4463, buf, RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH);
    osMutexRelease(rx_buffer_mutex);
    osMessagePut(rx_queue, (uint32_t)buf, 0);

    /* Clear RX FIFO */
    SI4463_ClearRxFifo(&si4463);

    /* Start RX again.
     * It need because after successful receive a packet the chip change
     * state to READY.
     * There is re-armed mode for StartRx but it not correctly working */
    SI4463_StartRx(&si4463, false, false, false);
  }

  SI4463_ClearAllInterrupts(&si4463);
}
