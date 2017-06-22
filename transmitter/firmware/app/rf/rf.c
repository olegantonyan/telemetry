#include "rf/rf.h"
#include "rf/radio_config_Si4463.h"
#include "serial_log/serial_log.h"
#include "cmsis_os.h"

static SPI_HandleTypeDef *spi_handle = NULL;
static GPIO_TypeDef *nsel_port = NULL;
static uint16_t nsel = 0;

static void nsel_activate();
static void nsel_deactivate();
static bool spi_communicate(const uint8_t *write, size_t write_length, uint8_t *read, size_t read_length);
static bool cts();
static bool wait_for_cts();

bool rf_init(SPI_HandleTypeDef *spi, GPIO_TypeDef *nsel_gpio, uint16_t nsel_pin) {
  spi_handle = spi;
  nsel_port = nsel_gpio;
  nsel = nsel_pin;

  uint8_t init_sequence[] = RADIO_CONFIGURATION_DATA_ARRAY;
  return spi_communicate(init_sequence, sizeof init_sequence, NULL, 0);
}

bool rf_transmit(const uint8_t *data, size_t length) {
  if (!spi_handle || !nsel_port) {
    return false;
  }

  if (!wait_for_cts()) {
    return false;
  }
  // TODO initialize tx and write fifo

  return spi_communicate(data, length, NULL, 0);
}

static bool spi_communicate(const uint8_t *write, size_t write_length, uint8_t *read, size_t read_length) {
  const size_t SPI_TIMEOUT = 500;

  // TODO try full duplex HAL_SPI_Transmit_Receive
  nsel_activate();
  HAL_StatusTypeDef write_result = HAL_OK;
  if (write && write_length > 0) {
    write_result = HAL_SPI_Transmit(spi_handle, (uint8_t *)write, sizeof write_length, SPI_TIMEOUT);
  }

  HAL_StatusTypeDef read_result = HAL_OK;
  if (read && read_length > 0) {
    read_result = HAL_SPI_Receive(spi_handle, read, read_length, SPI_TIMEOUT);
  }
  nsel_deactivate();

  return write_result == HAL_OK && read_result == HAL_OK;
}

static void nsel_activate() {
  HAL_GPIO_WritePin(nsel_port, nsel, GPIO_PIN_RESET);
}

static void nsel_deactivate() {
  HAL_GPIO_WritePin(nsel_port, nsel, GPIO_PIN_SET);
}

static bool cts() {
  const uint8_t data[] = { 0x44 };
  uint8_t read_byte = 0;
  if (!spi_communicate(data, sizeof data, &read_byte, sizeof read_byte)) {
    return false;
  }
  return read_byte == 0xFF;
}

static bool wait_for_cts() {
  const size_t MAX_CTS_RETRIES = 100;
  const size_t CTS_RETRY_DELAY = 10;

  size_t i = MAX_CTS_RETRIES;
  while (!cts() && i-- > 0) {
    osDelay(CTS_RETRY_DELAY);
  }
  return i > 0;
}
