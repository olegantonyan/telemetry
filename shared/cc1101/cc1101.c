#include "cc1101/cc1101.h"
#include "cc1101/smartrf_CC1100.h"
#include <stdio.h>
#include <string.h>

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
//CC1100 - config register
#define IOCFG2   0x00         // GDO2 output pin configuration
#define IOCFG1   0x01         // GDO1 output pin configuration
#define IOCFG0   0x02         // GDO0 output pin configuration
#define FIFOTHR  0x03         // RX FIFO and TX FIFO thresholds
#define SYNC1    0x04         // Sync word, high byte
#define SYNC0    0x05         // Sync word, low byte
#define PKTLEN   0x06         // Packet length
#define PKTCTRL1 0x07         // Packet automation control
#define PKTCTRL0 0x08         // Packet automation control
#define ADDR     0x09         // Device address
#define CHANNR   0x0A         // Channel number
#define FSCTRL1  0x0B         // Frequency synthesizer control
#define FSCTRL0  0x0C         // Frequency synthesizer control
#define FREQ2    0x0D         // Frequency control word, high byte
#define FREQ1    0x0E         // Frequency control word, middle byte
#define FREQ0    0x0F         // Frequency control word, low byte
#define MDMCFG4  0x10         // Modem configuration
#define MDMCFG3  0x11         // Modem configuration
#define MDMCFG2  0x12         // Modem configuration
#define MDMCFG1  0x13         // Modem configuration
#define MDMCFG0  0x14         // Modem configuration
#define DEVIATN  0x15         // Modem deviation setting
#define MCSM2    0x16         // Main Radio Cntrl State Machine config
#define MCSM1    0x17         // Main Radio Cntrl State Machine config
#define MCSM0    0x18         // Main Radio Cntrl State Machine config
#define FOCCFG   0x19         // Frequency Offset Compensation config
#define BSCFG    0x1A         // Bit Synchronization configuration
#define AGCCTRL2 0x1B         // AGC control
#define AGCCTRL1 0x1C         // AGC control
#define AGCCTRL0 0x1D         // AGC control
#define WOREVT1  0x1E         // High byte Event 0 timeout
#define WOREVT0  0x1F         // Low byte Event 0 timeout
#define WORCTRL  0x20         // Wake On Radio control
#define FREND1   0x21         // Front end RX configuration
#define FREND0   0x22         // Front end TX configuration
#define FSCAL3   0x23         // Frequency synthesizer calibration
#define FSCAL2   0x24         // Frequency synthesizer calibration
#define FSCAL1   0x25         // Frequency synthesizer calibration
#define FSCAL0   0x26         // Frequency synthesizer calibration
#define RCCTRL1  0x27         // RC oscillator configuration
#define RCCTRL0  0x28         // RC oscillator configuration
#define FSTEST   0x29         // Frequency synthesizer cal control
#define PTEST    0x2A         // Production test
#define AGCTEST  0x2B         // AGC test
#define TEST2    0x2C         // Various test settings
#define TEST1    0x2D         // Various test settings
#define TEST0    0x2E         // Various test settings

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

#define CC1101_MAX_PACKET_LENGTH 60

static CC1101_t config;

static void reset();
static uint8_t strobe(uint8_t byte);
static uint8_t read_register(uint8_t addr);
static void read_burst(uint8_t addr, uint8_t *buffer, size_t length);
static void write_register(uint8_t addr, uint8_t value);
static void write_burst(uint8_t addr, const uint8_t *buffer, size_t length);
static void start_rx();
static void start_tx();
static void read_rx_fifo(uint8_t *buffer, size_t length);
static void write_tx_fifo(const uint8_t *buffer, size_t length);
static void write_pa_table(const uint8_t *pa_table);
static void write_rf_settings(const uint8_t *settings, size_t length);
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

  /*uint8_t status = strobe(0);
  printf("status = 0x%X R/W=0\n", status);
  status = strobe(0x80);
  printf("status = 0x%X R/W=1\n", status);
*/
  uint8_t partnum = read_register(PARTNUM);
  uint8_t version = read_register(VERSION);
  printf("version = 0x%X, partnum = 0x%X\n", version, partnum); //checks if valid Chip ID is found. Usualy 0x03 or 0x14

  write_rf_settings(RF_SETTINGS, sizeof RF_SETTINGS);
  write_pa_table(PA_TABLE);

  printf("RCCTRL1_STATUS = 0x%X, RCCTRL0_STATUS = 0x%X\n", read_register(RCCTRL1_STATUS), read_register(RCCTRL0_STATUS));

  start_rx();

  return true;
}

bool cc1101_transmit(const uint8_t *data, uint16_t length) {
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
  // 0xFE to read single
  write_burst(0x7E, pa_table, 8); // always 8 bytes
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

static uint8_t strobe(uint8_t byte) {
  config.chip_select(true);
  config.wait_chip_ready();
  uint8_t rx = 0;
  config.write_read(&byte, &rx, sizeof(byte));
  config.chip_select(false);
  return rx;
}

static uint8_t read_register(uint8_t addr) {
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
