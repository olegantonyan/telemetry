#include "cc1101/cc1101.h"
#include "cc1101/smartrf_CC1100.h"
#include <stdio.h>

typedef enum {
  SRES     = 0x30,         // Reset chip
  SFSTXON  = 0x31,         // Enable/calibrate freq synthesizer
  SXOFF    = 0x32,         // Turn off crystal oscillator.
  SCAL     = 0x33,         // Calibrate freq synthesizer & disable
  SRX      = 0x34,         // Enable RX.
  STX      = 0x35,         // Enable TX.
  SIDLE    = 0x36,         // Exit RX / TX
  SAFC     = 0x37,         // AFC adjustment of freq synthesizer
  SWOR     = 0x38,         // Start automatic RX polling sequence
  SPWD     = 0x39,         // Enter pwr down mode when CSn goes hi
  SFRX     = 0x3A,         // Flush the RX FIFO buffer.
  SFTX     = 0x3B,         // Flush the TX FIFO buffer.
  SWORRST  = 0x3C,         // Reset real time clock.
  SNOP     = 0x3D         // No operation.
} CC1101_COMMAND_STROBE;

typedef enum {
  // config
  IOCFG2   = 0x00,         // GDO2 output pin configuration
  IOCFG1   = 0x01,         // GDO1 output pin configuration
  IOCFG0   = 0x02,         // GDO0 output pin configuration
  FIFOTHR  = 0x03,         // RX FIFO and TX FIFO thresholds
  SYNC1    = 0x04,         // Sync word, high byte
  SYNC0    = 0x05,         // Sync word, low byte
  PKTLEN   = 0x06,         // Packet length
  PKTCTRL1 = 0x07,         // Packet automation control
  PKTCTRL0 = 0x08,         // Packet automation control
  ADDR     = 0x09,         // Device address
  CHANNR   = 0x0A,        // Channel number
  FSCTRL1  = 0x0B,         // Frequency synthesizer control
  FSCTRL0  = 0x0C,         // Frequency synthesizer control
  FREQ2    = 0x0D,         // Frequency control word, high byte
  FREQ1    = 0x0E,         // Frequency control word, middle byte
  FREQ0    = 0x0F,         // Frequency control word, low byte
  MDMCFG4  = 0x10,         // Modem configuration
  MDMCFG3  = 0x11,         // Modem configuration
  MDMCFG2  = 0x12,         // Modem configuration
  MDMCFG1  = 0x13,         // Modem configuration
  MDMCFG0  = 0x14,         // Modem configuration
  DEVIATN  = 0x15,         // Modem deviation setting
  MCSM2    = 0x16,         // Main Radio Cntrl State Machine config
  MCSM1    = 0x17,        // Main Radio Cntrl State Machine config
  MCSM0    = 0x18,         // Main Radio Cntrl State Machine config
  FOCCFG   = 0x19,         // Frequency Offset Compensation config
  BSCFG    = 0x1A,         // Bit Synchronization configuration
  AGCCTRL2 = 0x1B,         // AGC control
  AGCCTRL1 = 0x1C,         // AGC control
  AGCCTRL0 = 0x1D,         // AGC control
  WOREVT1  = 0x1E,         // High byte Event 0 timeout
  WOREVT0  = 0x1F,         // Low byte Event 0 timeout
  WORCTRL  = 0x20,         // Wake On Radio control
  FREND1   = 0x21,         // Front end RX configuration
  FREND0   = 0x22,         // Front end TX configuration
  FSCAL3   = 0x23,         // Frequency synthesizer calibration
  FSCAL2   = 0x24,         // Frequency synthesizer calibration
  FSCAL1   = 0x25,         // Frequency synthesizer calibration
  FSCAL0   = 0x26,         // Frequency synthesizer calibration
  RCCTRL1  = 0x27,         // RC oscillator configuration
  RCCTRL0  = 0x28,         // RC oscillator configuration
  FSTEST   = 0x29,         // Frequency synthesizer cal control
  PTEST    = 0x2A,         // Production test
  AGCTEST  = 0x2B,         // AGC test
  TEST2    = 0x2C,         // Various test settings
  TEST1    = 0x2D,         // Various test settings
  TEST0    = 0x2E,         // Various test settings
  // status
  PARTNUM        = 0xF0,   // Part number
  VERSION        = 0xF1,   // Current version number
  FREQEST        = 0xF2,   // Frequency offset estimate
  LQI            = 0xF3,   // Demodulator estimate for link quality
  RSSI           = 0xF4,   // Received signal strength indication
  MARCSTATE      = 0xF5,   // Control state machine state
  WORTIME1       = 0xF6,   // High byte of WOR timer
  WORTIME0       = 0xF7,   // Low byte of WOR timer
  PKTSTATUS      = 0xF8,   // Current GDOx status and packet status
  VCO_VC_DAC     = 0xF9,   // Current setting from PLL cal module
  TXBYTES        = 0xFA,   // Underflow and # of bytes in TXFIFO
  RXBYTES        = 0xFB,   // Overflow and # of bytes in RXFIFO
  RCCTRL1_STATUS = 0xFC,   //Last RC Oscillator Calibration Result
  RCCTRL0_STATUS = 0xFD   //Last RC Oscillator Calibration Result
} CC1101_REGISTER;

typedef enum {
  SLEEP             = 0,   // SLEEP
  IDLE              = 1,   // IDLE
  XOFF              = 2,   // XOFF
  VCOON_MC          = 3,   // MANCAL
  REGON_MC          = 4,   // MANCAL
  MANCAL            = 5,   // MANCAL
  VCOON             = 6,   // FS_WAKEUP
  REGON             = 7,   // FS_WAKEUP
  STARTCAL          = 8,   // CALIBRATE
  BWBOOST           = 9,   // SETTLING
  FS_LOCK           = 10,  // SETTLING
  IFADCON           = 11,  // SETTLING
  ENDCAL            = 12,  // CALIBRATE
  RX                = 13,  // RX
  RX_END            = 14,  // RX
  RX_RST            = 15,  // RX
  TXRX_SWITCH       = 16,  // TXRX_SETTLING
  RXFIFO_OVERFLOW   = 17,  // RXFIFO_OVERFLOW
  FSTXON            = 18,  // FSTXON
  TX                = 19,  // TX
  TX_END            = 20,  // TX
  RXTX_SWITCH       = 21,  // RXTX_SETTLING
  TXFIFO_UNDERFLOW  = 22,  // TXFIFO_UNDERFLOW
} CC1101_MARCSTATE;

#define CC1101_MAX_PACKET_LENGTH 60 // TODO: handle packets > fifo

static CC1101_t config;

static void reset();
static uint8_t strobe(CC1101_COMMAND_STROBE byte);
static uint8_t read_register(CC1101_REGISTER addr);
static void read_burst(uint8_t addr, uint8_t *buffer, size_t length);
static void write_register(uint8_t addr, uint8_t value);
static void write_burst(uint8_t addr, const uint8_t *buffer, size_t length);
static void start_rx();
static void start_tx();
static void read_rx_fifo(uint8_t *buffer, size_t length);
static void write_tx_fifo(const uint8_t *buffer, size_t length);
static void write_pa_table(const uint8_t *pa_table);
static void read_pa_table(uint8_t *pa_table);
static void write_rf_settings(const uint8_t *settings, size_t length);
static void read_rf_settings(uint8_t *settings, size_t length);
static void sidle();
static void flush_tx_fifo();
static void flush_rx_fifo();
static bool is_crc_ok();
static bool wait_for_marcstate(CC1101_MARCSTATE expected_state);

bool cc1101_init(const CC1101_t *c) {
  if (!c) {
    return false;
  }
  config = *c;

  reset();
  flush_rx_fifo();
  flush_tx_fifo();
  write_rf_settings(RF_SETTINGS, sizeof RF_SETTINGS);
  write_pa_table(PA_TABLE);

#if DEBUG
  printf("CC1101: PARTNUM = 0x%02X\n", read_register(PARTNUM));
  printf("CC1101: VERSION = 0x%02X\n", read_register(VERSION));

  uint8_t rf_settings[sizeof(RF_SETTINGS)] = { 0 };
  read_rf_settings(rf_settings, sizeof rf_settings);
  for (size_t i = 0; i < sizeof rf_settings; i++) {
    printf("CC1101: RF_SETTINGS @ 0x%02X = 0x%02X\n", i, rf_settings[i]);
  }

  uint8_t pa_table[8] = { 0 };
  read_pa_table(pa_table);
  for (size_t i = 0; i < sizeof pa_table; i++) {
    printf("CC1101: PATABLE @ 0x%02X = 0x%02X\n", i, pa_table[i]);
  }
#endif

  start_rx();

  return true;
}

bool cc1101_transmit(const uint8_t *data, uint16_t length) {
  // TODO wait for RX finished if it's in progress
  if (length > CC1101_MAX_PACKET_LENGTH) {
    return false;
  }

  sidle();
  flush_tx_fifo();

  uint8_t len[1] = { length };
  write_tx_fifo(len, 1);
  write_tx_fifo(data, length);
  start_tx();

  return true;
}

void cc1101_gdo_interrupt() {
  uint8_t bytes_available = read_register(RXBYTES);
  if ((bytes_available & 0x7F) && !(bytes_available & 0x80) && bytes_available <= CC1101_MAX_PACKET_LENGTH && is_crc_ok()) {
    uint8_t data[CC1101_MAX_PACKET_LENGTH] = { 0 };
    read_rx_fifo(data, bytes_available);
    config.packet_received(data, bytes_available);
  }

  CC1101_MARCSTATE state = read_register(MARCSTATE);
  if (state == TX_END) {
    config.packet_sent();
  }
  start_rx();
}

static void write_rf_settings(const uint8_t *settings, size_t length) {
  write_burst(0, settings, length);
}

static void write_pa_table(const uint8_t *pa_table) {
  write_burst(0x7E, pa_table, 8); // always 8 bytes
}

static void read_pa_table(uint8_t *pa_table) {
  // 0xFE to read single
  read_burst(0x7E, pa_table, 8);
}

static void read_rf_settings(uint8_t *settings, size_t length) {
  read_burst(0, settings, length);
}

static void flush_tx_fifo() {
  strobe(SFTX);
  wait_for_marcstate(IDLE);
}

static void flush_rx_fifo() {
  strobe(SFRX);
  wait_for_marcstate(IDLE);
}

static void sidle() {
  strobe(SIDLE);
  wait_for_marcstate(IDLE);
}

static bool is_crc_ok() {
  uint8_t status = read_register(PKTSTATUS);
  return (status & 0x80) == 0x80;
}

static void read_rx_fifo(uint8_t *buffer, size_t length) {
  if (length == 1) {
    *buffer = read_register(0xBF);
  } else {
    read_burst(0xFF, buffer, length);
  }
}

static void write_tx_fifo(const uint8_t *buffer, size_t length) {
  if (length == 1) {
    write_register(0x3F, buffer[0]);
  } else {
    write_burst(0x7F, buffer, length);
  }
}

static void start_tx() {
  strobe(STX);
}

static void start_rx() {
  sidle();
  flush_rx_fifo();
  strobe(SRX);
  wait_for_marcstate(RX);
}

static bool wait_for_marcstate(CC1101_MARCSTATE expected_state) {
  uint8_t state = 0;
  do {
    state = read_register(MARCSTATE);
  } while(state != expected_state);
  return true;
}

static void reset() {
  config.chip_select(true);
  config.chip_select(false);
  strobe(SRES);
}

static uint8_t strobe(CC1101_COMMAND_STROBE byte) {
  config.chip_select(true);
  config.wait_chip_ready();
  uint8_t rx = 0;
  config.write_read(&byte, &rx, 1);
  config.chip_select(false);
  return rx;
}

static uint8_t read_register(CC1101_REGISTER addr) {
  config.chip_select(true);
  config.wait_chip_ready();
  const uint8_t tx[2] = { addr | 0x80, 0xFF };
  uint8_t rx[2] = { 0 };
  config.write_read(tx, rx, sizeof(tx));
  config.chip_select(false);
  return rx[1];
}

static void read_burst(uint8_t addr, uint8_t *buffer, size_t length) {
  config.chip_select(true);
  config.wait_chip_ready();
  uint8_t tx = addr | 0xC0;
  config.write_read(&tx, NULL, sizeof(tx));

  tx = 0xFF;
  config.write_read(&tx, buffer, length);

  config.chip_select(false);
}

static void write_register(uint8_t addr, uint8_t value) {
  config.chip_select(true);
  config.wait_chip_ready();
  uint8_t tx[2] = { addr | 0x00, value };
  config.write_read(tx, NULL, sizeof(tx));
  config.chip_select(false);
}

static void write_burst(uint8_t addr, const uint8_t *buffer, size_t length) {
  config.chip_select(true);
  config.wait_chip_ready();
  uint8_t tx = addr | 0x40;
  config.write_read(&tx, NULL, sizeof(tx));

  config.write_read(buffer, NULL, length);

  config.chip_select(false);
}
