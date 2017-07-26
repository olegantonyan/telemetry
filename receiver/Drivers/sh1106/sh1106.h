#ifndef __SH1106_H
#define __SH1106_H

#include "stm32f1xx_hal.h"

void sh1106_init(I2C_HandleTypeDef *i2c, void (*watchdog_kick_function)(void));
void sh1106_set_pixel(uint16_t x, uint16_t y, uint8_t value);
void sh1106_render();
void sh1106_draw_character(char character, uint16_t x_offset, uint16_t y_offset);
void sh1106_draw_rectangle(uint8_t x_size, uint8_t y_size, uint16_t x_offset, uint16_t y_offset);

#endif
