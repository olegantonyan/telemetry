#include <string.h>

#include "rf/rf.h"
#include "serial_log/serial_log.h"
#include "cmsis_os.h"
#include "si4463.h"
#include "main.h"

static si4463_t si4463;
extern SPI_HandleTypeDef hspi1;

static bool si4463_cts(void);
static void si4463_write_read(uint8_t *tx_data, uint8_t *rx_data, uint16_t length);
static void si4463_set_shutdown(void);
static void si4463_clear_shutdown(void);
static void si4463_select(void);
static void si4463_deselect(void);
static void fake_delay(uint32_t);

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
}

void rf_transmit(const uint8_t *data) {
  SI4463_Transmit(&si4463, (uint8_t *)data, RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH);
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
  }

  SI4463_ClearAllInterrupts(&si4463);
}
