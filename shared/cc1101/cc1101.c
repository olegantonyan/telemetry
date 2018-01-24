#include "cc1101/cc1101.h"
#include "cc1101/smartrf_CC1100.h"
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

/*------------------------[CC1100 - FIFO commands]----------------------------*/
#define TXFIFO_BURST        0x7F    //write burst only
#define TXFIFO_SINGLE_BYTE  0x3F    //write single only
#define RXFIFO_BURST        0xFF    //read burst only
#define RXFIFO_SINGLE_BYTE  0xBF    //read single only
#define PATABLE_BURST       0x7E    //power control read/write
#define PATABLE_SINGLE_BYTE 0xFE    //power control read/write
/*---------------------------[END FIFO commands]------------------------------*/

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

static CC1101_t config;

static void reset();
static uint8_t strobe(uint8_t byte);
static uint8_t read_register(uint8_t addr);
static void read_burst(uint8_t addr, uint8_t *buffer, size_t length);
static void write_register(uint8_t addr, uint8_t value);
static void write_burst(uint8_t addr, const uint8_t *buffer, size_t length);
static void start_receive();
static bool wait_for_marcstate(CC1101_MARCSTATE expected_state);

bool cc1101_init(const CC1101_t *c) {
  if (!c) {
    return false;
  }
  config = *c;

  reset();

  strobe(SFTX);
  wait_for_marcstate(IDLE);
  strobe(SFRX);
  wait_for_marcstate(IDLE);

  uint8_t status = strobe(0);
  printf("status = 0x%X R/W=0\n", status);
  status = strobe(0x80);
  printf("status = 0x%X R/W=1\n", status);

  uint8_t partnum = read_register(PARTNUM);
  uint8_t version = read_register(VERSION);
  printf("version = 0x%X, partnum = 0x%X\n", version, partnum); //checks if valid Chip ID is found. Usualy 0x03 or 0x14

  write_burst(0, RF_SETTINGS, sizeof(RF_SETTINGS));

  printf("RCCTRL1_STATUS = 0x%X, RCCTRL0_STATUS = 0x%X\n", read_register(RCCTRL1_STATUS), read_register(RCCTRL0_STATUS));

  start_receive();

  return true;
}

bool cc1101_transmit(const uint8_t *data, uint16_t length) {
  strobe(SIDLE);
  wait_for_marcstate(IDLE);
  strobe(SFTX);
  wait_for_marcstate(IDLE);

  write_burst(0x7F, data, length); // TX FIFO write

  //config.delay(10);
  printf("MARCSTATE after tx fifo write = %d\n", read_register(MARCSTATE));

  //strobe(SIDLE);
  //wait_for_marcstate(IDLE);

  strobe(STX); // start TX
  config.delay(100);
  //wait_for_marcstate(0x13); // wait for TX finished
  printf("MARCSTATE after STX = %d\n", read_register(MARCSTATE));

  //printf("MCSM1 after STX = 0x%X\n", read_register(MCSM1));

  return true;
}

static void start_receive() {
  strobe(SIDLE);
  wait_for_marcstate(IDLE);
  strobe(SRX);
  wait_for_marcstate(RX);
  printf("MARCSTATE after SRX = %d\n", read_register(MARCSTATE));
}

static bool wait_for_marcstate(uint8_t expected_state) {
  uint8_t state = 0;
  do {
    state = read_register(MARCSTATE);
  } while(state != expected_state);
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
