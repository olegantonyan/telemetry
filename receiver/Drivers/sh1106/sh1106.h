#ifndef __SH1106_H
#define __SH1106_H

#include <stdint.h>

void sh1106_init(void (*i2c_transmit_fn)(uint16_t addr, uint8_t *data, uint16_t size));
void sh1106_set_pixel(uint16_t x, uint16_t y, uint8_t value);
void sh1106_render();
void sh1106_draw_character(char character, uint16_t x_offset, uint16_t y_offset);
void sh1106_draw_rectangle(uint8_t x_size, uint8_t y_size, uint16_t x_offset, uint16_t y_offset);
void sh1106_draw_string(const char *string, uint16_t x_offset, uint16_t y_offset);

#endif
