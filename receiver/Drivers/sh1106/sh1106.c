#include <string.h>

#include "sh1106.h"

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
