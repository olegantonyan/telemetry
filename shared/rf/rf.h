#ifndef __RF_H
#define __RF_H

#include <stdint.h>

#include "si4463/si4463.h"

#define RF_PACKET_LENGTH (RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH - 4) // 4 bytes for software CRC

void rf_init();
void rf_transmit(const uint8_t *data);
bool rf_receive(uint8_t * data);

#endif
