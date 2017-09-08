#ifndef _CC1101_H_
#define _CC1101_H_

#include <stdint.h>
#include <stdbool.h>

#include "cc1101/smartrf_CC1100.h"

typedef struct {
  void (*write_read)(const uint8_t *tx_data, uint8_t *rx_data, uint16_t length);
	void (*chip_select)(bool select);
} CC1101_t;

bool cc1101_init(const CC1101_t *config);

#endif
