#ifndef _CC1101_H_
#define _CC1101_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  void (*write_read)(const uint8_t *tx_data, uint8_t *rx_data, uint16_t length);
	void (*chip_select)(bool select);
  void (*wait_chip_ready)();
  void (*packet_received)(uint16_t length);
  void (*packet_sent)();
} CC1101_t;

bool cc1101_init(const CC1101_t *config);
bool cc1101_transmit(const uint8_t *data, uint16_t length);
bool cc1101_read_received_data(uint8_t *data, uint16_t length);
void cc1101_gdo_interrupt();

#endif
