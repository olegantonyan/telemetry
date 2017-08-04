#include <string.h>

#include "sh1106.h"
#include "fonts.h"

static I2C_HandleTypeDef *i2c = NULL;
static uint8_t buffer[128 * 64 / 8] = { 0 };

static const uint8_t SH1106_I2C_ADDRESS = 0x78;
static const uint8_t I2C_TIMEOUT = 10;

static void init();
static void send_command(uint8_t cmd);
static void send_data(uint8_t data);
static void clear();
static void refresh();

void sh1106_init(I2C_HandleTypeDef *i2c_handle) {
  i2c = i2c_handle;
  init();
  clear();
}

void sh1106_set_pixel(uint16_t x, uint16_t y, uint8_t value) {
  /*
  buffer[i] => column, i bits => rows
  buffer[0] => first column, 0x1 => top left pixel
  */
  if (value != 0) {
    buffer[x + (y / 8) * 128] |=  (1 << (y & 7));
  } else {
    buffer[x + (y / 8) * 128] &=  ~(1 << (y & 7));
  }
}

void sh1106_draw_character(char character, uint16_t x_offset, uint16_t y_offset) {
  const uint8_t *font = sh1106_font(character);
  if (!font) {
    return;
  }
  for (uint8_t y = 0; y < 30; y++) {
    for (uint8_t x = 0; x < 24; x++) {
      uint8_t value = font[x / 8 + y * 3] >> (7 - x % 8);
      sh1106_set_pixel(x_offset + x, y_offset + y, value & 0x01);
    }
  }
}

void sh1106_draw_rectangle(uint8_t x_size, uint8_t y_size, uint16_t x_offset, uint16_t y_offset) {
  for (uint8_t x = 0; x < x_size; x++) {
    for (uint8_t y = 0; y < y_size; y++) {
      sh1106_set_pixel(x_offset + x, y_offset + y, 1);
    }
  }
}

void sh1106_draw_string(const char *string, uint16_t x_offset, uint16_t y_offset) {
  char c;
  while ((c = *string++) != '\0') {
    if (c == '.') {
      sh1106_draw_rectangle(5, 5, x_offset + 1, y_offset + 25);
      x_offset += 9;
    } else {
      sh1106_draw_character(c, x_offset, y_offset);
      x_offset += 26;
    }
  }
}

void sh1106_render() {
  refresh();
}

static void init() {
  send_command(0xAE); //0xae, Display OFF

  send_command(0x02); //0x02,Low Column shifted because the RAM has 132 columns
  send_command(0x10); //0x10,//High Column

  send_command(0x40); // 0x40, line #0, start line

  send_command(0xB0); // set page address

  send_command(0x81); // 0x81, set contrast
  send_command(0x80);

  send_command(0xA1); //0xA1,//remap

  send_command(0xA4); // 0xA4
  send_command(0xA6); // 0xA6, normal display

  send_command(0xA8); // 0xA8, multiplex ratio
  send_command(0x3F); // 63, 1/64 duty

  send_command(0xAD); // DC-DC ON
  send_command(0x8B);

  send_command(0x33); // VPP=9.0V

  send_command(0xc8); // scan direction

  send_command(0xD3); // 0xD3, display offset
  send_command(0x0); // no offset

  send_command(0xD5); // 0xD5, Display clock divide
  send_command(0x80); // the suggested ratio 0x80

  send_command(0xD9); // 0xd9, precharge period
  send_command(0x22);

  send_command(0xDA); // 0xDA, com pins
  send_command(0x12);

  send_command(0xDB); // 0xDB, VCOM deselect
  send_command(0x40);

//  HAL_Delay(100);

  send_command(0xAF); //0xAF, display ON

//  HAL_Delay(100);
}

static void clear() {
  memset(buffer, 0, sizeof buffer);
  refresh();
}

static void refresh() {
  for(size_t row = 0; row < 8; row++) {
    send_command(0xB0 + row);
    send_command(0x02);
    send_command(0x10);
    for(size_t column = 0; column < 128; column++) {
      send_data(buffer[row * 128 + column]);
    }
  }
}

static void send_command(uint8_t cmd) {
  uint8_t buf[] = { 0x80, cmd };
  HAL_I2C_Master_Transmit(i2c, SH1106_I2C_ADDRESS, buf, sizeof buf, I2C_TIMEOUT);
}

static void send_data(uint8_t data) {
  uint8_t buf[] = { 0x40, data };
  HAL_I2C_Master_Transmit(i2c, SH1106_I2C_ADDRESS, buf, sizeof buf, I2C_TIMEOUT);
}
