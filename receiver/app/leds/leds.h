#ifndef __LEDS_H
#define __LEDS_H

#include "stm32f1xx_hal.h"

void leds_status_flash(uint32_t ms);
void leds_status_on();
void leds_status_off();

#endif
