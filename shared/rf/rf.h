#ifndef __RF_H
#define __RF_H

#include <stdint.h>

#define RF_PACKET_LENGTH 12 // +4 bytes for software CRC
#if (RF_PACKET_LENGTH % 4 != 0)
  #error "RF_PACKET_LENGTH must be 4 bytes aligned"
#endif

void rf_init();
void rf_transmit(const uint8_t *data);
bool rf_receive(uint8_t * data, uint32_t timeout_ms);

#endif
