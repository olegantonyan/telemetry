#ifndef _RF_PROTOCOL_
#define _RF_PROTOCOL_

#include <stdint.h>

#include "stm32f1xx_hal.h"

uint8_t rf_init();
uint8_t on_rf_phy_byte_received(uint8_t byte);

#endif
