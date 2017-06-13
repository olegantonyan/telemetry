#ifndef _EMULATED_KEYBOARD_
#define _EMULATED_KEYBOARD_

#include <stdint.h>

enum KEYCODES {
  ENTER = 0x28,
  ARROW_UP = 0x52,
  ARROW_DOWN = 0x51,
  ARROW_LEFT = 0x50,
  ARROW_RIGHT = 0x4F
};

void emulated_keyboard_write_char(uint8_t byte);

#endif
