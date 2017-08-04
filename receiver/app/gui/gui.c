#include <stdio.h>

#include "gui/gui.h"
#include "cmsis_os.h"
#include "sh1106.h"
#include "stm32f1xx_hal.h"

static void thread(void const *arg);
static osThreadId thread_handle;
static void i2c_transmit(uint16_t addr, uint8_t *data, uint16_t size);
static osSemaphoreId i2c_tx_semaphore;
osSemaphoreDef(i2c_tx_semaphore);

static volatile uint8_t voltage_integer = 0;
static volatile uint16_t voltage_fractional = 0;
static volatile uint8_t current_integer = 0;
static volatile uint16_t current_fractional = 0;

extern I2C_HandleTypeDef hi2c1;

void gui_init() {
  osThreadDef(gui_thread, thread, osPriorityNormal, 0, 128);
  i2c_tx_semaphore = osSemaphoreCreate(osSemaphore(i2c_tx_semaphore), 1);
  thread_handle = osThreadCreate(osThread(gui_thread), NULL);
}

void gui_display_voltage(uint8_t integer, uint16_t fractional) {
  voltage_integer = integer;
  voltage_fractional = fractional;
}

void gui_display_current(uint8_t integer, uint16_t fractional) {
  current_integer = integer;
  current_fractional = fractional;
}

static void thread(void const *arg) {
  sh1106_init(&i2c_transmit);

  while(true) {
    char string[6] = { 0 };
    snprintf(string, sizeof string, "%02u.%02u", voltage_integer, voltage_fractional);
    sh1106_draw_string(string, 0, 0);

    snprintf(string, sizeof string, "%03u.%01u", current_integer, current_fractional);
    sh1106_draw_string(string, 0, 33);

    sh1106_render();
    osDelay(200);
  }
}

static void i2c_transmit(uint16_t addr, uint8_t *data, uint16_t size) {
  HAL_I2C_Master_Transmit_DMA(&hi2c1, addr, data, size);
  osSemaphoreWait(i2c_tx_semaphore, osWaitForever);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
  osSemaphoreRelease(i2c_tx_semaphore);
}
