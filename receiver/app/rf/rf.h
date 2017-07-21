#ifndef __RF_H
#define __RF_H

#include "stm32f1xx_hal.h"

void rf_init();
void rf_transmit(const uint8_t *data);
bool rf_receive(uint8_t * data);

#endif
