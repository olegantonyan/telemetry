#ifndef __RF_H
#define __RF_H

#include <stdint.h>

#define RF_PACKET_LENGTH 12

void rf_init();
bool rf_transmit(const uint8_t *data);
bool rf_receive(uint8_t * data, uint32_t timeout_ms);

#endif
