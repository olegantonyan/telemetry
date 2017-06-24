#ifndef __RF_H
#define __RF_H

#include "stm32f1xx_hal.h"

bool rf_init(SPI_HandleTypeDef *spi, GPIO_TypeDef *gpio_port, uint16_t nsel_pin);
bool rf_transmit(const uint8_t * data, size_t length);

#endif
