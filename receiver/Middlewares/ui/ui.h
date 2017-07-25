#ifndef __UI_H
#define __UI_H

#include <stdint.h>

typedef struct {
  void (*set_pixel_function)(uint16_t x, uint16_t y, uint8_t value);
  void (*render_function);
} UI_Config;

void ui_init(UI_Config cfg);

#endif
