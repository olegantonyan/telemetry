#include "cc1101/cc1101.h"
#include <stdio.h>

// CC1100-command strobes
#define SRES     0x30         // Reset chip
#define SFSTXON  0x31         // Enable/calibrate freq synthesizer
#define SXOFF    0x32         // Turn off crystal oscillator.
#define SCAL     0x33         // Calibrate freq synthesizer & disable
#define SRX      0x34         // Enable RX.
#define STX      0x35         // Enable TX.
#define SIDLE    0x36         // Exit RX / TX
#define SAFC     0x37         // AFC adjustment of freq synthesizer
#define SWOR     0x38         // Start automatic RX polling sequence
#define SPWD     0x39         // Enter pwr down mode when CSn goes hi
#define SFRX     0x3A         // Flush the RX FIFO buffer.
#define SFTX     0x3B         // Flush the TX FIFO buffer.
#define SWORRST  0x3C         // Reset real time clock.
#define SNOP     0x3D         // No operation.
//CC1100 - status register
#define PARTNUM        0xF0   // Part number
#define VERSION        0xF1   // Current version number
#define FREQEST        0xF2   // Frequency offset estimate
#define LQI            0xF3   // Demodulator estimate for link quality
#define RSSI           0xF4   // Received signal strength indication
#define MARCSTATE      0xF5   // Control state machine state
#define WORTIME1       0xF6   // High byte of WOR timer
#define WORTIME0       0xF7   // Low byte of WOR timer
#define PKTSTATUS      0xF8   // Current GDOx status and packet status
#define VCO_VC_DAC     0xF9   // Current setting from PLL cal module
#define TXBYTES        0xFA   // Underflow and # of bytes in TXFIFO
#define RXBYTES        0xFB   // Overflow and # of bytes in RXFIFO
#define RCCTRL1_STATUS 0xFC   //Last RC Oscillator Calibration Result
#define RCCTRL0_STATUS 0xFD   //Last RC Oscillator Calibration Result
//CC1100 - R/W offsets
#define WRITE_SINGLE_BYTE   0x00
#define WRITE_BURST         0x40
#define READ_SINGLE_BYTE    0x80
#define READ_BURST          0xC0

static CC1101_t config;

static void reset();
static uint8_t strobe(uint8_t byte);
static uint8_t read_register(uint8_t addr);
static void read_burst(uint8_t addr, uint8_t *buffer, size_t length);

bool cc1101_init(const CC1101_t *c) {
  config = *c;

  reset();

  strobe(SFTX);
  strobe(SFRX);

  uint8_t partnum = read_register(PARTNUM);
  uint8_t version = read_register(VERSION);

  printf("version = 0x%X, partnum = 0x%X\n", version, partnum); //checks if valid Chip ID is found. Usualy 0x03 or 0x14. if not -> abort

  return true;
}

static void reset() {
  config.chip_select(true);
  config.chip_select(false);
  config.delay(1);
  strobe(SRES);
}

static uint8_t strobe(uint8_t byte) {
  config.chip_select(true);
  uint8_t rx = 0;
  config.write_read(&byte, &rx, sizeof(byte));
  config.chip_select(false);
  return rx;
}

static uint8_t read_register(uint8_t addr) {
  config.chip_select(true);
  const uint8_t tx[2] = { addr | READ_SINGLE_BYTE, 0xFF };
  uint8_t rx[2] = { 0 };
  config.write_read(tx, rx, sizeof(tx));
  config.chip_select(false);
  return rx[1];
}

static void read_burst(uint8_t addr, uint8_t *buffer, size_t length) {
  config.chip_select(true);

  uint8_t tx = addr | READ_BURST;
  uint8_t rx = 0;
  config.write_read(&tx, &rx, sizeof(tx));

  tx = 0xFF;
  config.write_read(&tx, buffer, length);

  config.chip_select(false);
}
