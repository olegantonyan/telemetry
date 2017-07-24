#ifndef __SH1106_H
#define __SH1106_H

#include "stm32f1xx_hal.h"

typedef struct {
  uint16_t x;
  uint16_t y;
  uint8_t value;
} SH1106_Pixel;

void sh1106_init(I2C_HandleTypeDef *i2c);
void sh1106_write(SH1106_Pixel pixel);
void sh1106_refresh();

#endif
