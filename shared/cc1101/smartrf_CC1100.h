/***************************************************************
 *  SmartRF Studio(tm) Export
 *
 *  Radio register settings specifed with C-code
 *  compatible #define statements.
 *
 *  RF device: CC1100
 *
 ***************************************************************/

#ifndef SMARTRF_CC1100_H
#define SMARTRF_CC1100_H

/* Address Config = No address check */
/* Base Frequency = 867.999939 */
/* CRC Autoflush = false */
/* CRC Enable = true */
/* Carrier Frequency = 867.999939 */
/* Channel Number = 0 */
/* Channel Spacing = 199.951172 */
/* Data Format = Normal mode */
/* Data Rate = 249.939 */
/* Device Address = 0 */
/* Manchester Enable = false */
/* Modulated = true */
/* Modulation Format = MSK */
/* PA Ramping = false */
/* Packet Length = 255 */
/* Packet Length Mode = Variable packet length mode. Packet length configured by the first byte after sync word */
/* Phase Transition Time = 0 */
/* Preamble Count = 4 */
/* RX Filter BW = 541.666667 */
/* Sync Word Qualifier Mode = 30/32 sync word bits detected */
/* TX Power = 0 */
/* Whitening = false */
/* PA table */
#define PA_TABLE {0x8e,0x00,0x00,0x00,0x00,0x00,0x00,0x00}

#define SMARTRF_RADIO_CC1100
#define SMARTRF_SETTING_IOCFG0     0x06
#define SMARTRF_SETTING_PKTCTRL0   0x05
#define SMARTRF_SETTING_FSCTRL1    0x0A
#define SMARTRF_SETTING_FREQ2      0x21
#define SMARTRF_SETTING_FREQ1      0x62
#define SMARTRF_SETTING_FREQ0      0x76
#define SMARTRF_SETTING_MDMCFG4    0x2D
#define SMARTRF_SETTING_MDMCFG3    0x3B
#define SMARTRF_SETTING_MDMCFG2    0x73
#define SMARTRF_SETTING_DEVIATN    0x00
#define SMARTRF_SETTING_MCSM0      0x18
#define SMARTRF_SETTING_FOCCFG     0x1D
#define SMARTRF_SETTING_BSCFG      0x1C
#define SMARTRF_SETTING_AGCCTRL2   0xC7
#define SMARTRF_SETTING_AGCCTRL1   0x00
#define SMARTRF_SETTING_AGCCTRL0   0xB0
#define SMARTRF_SETTING_FREND1     0xB6
#define SMARTRF_SETTING_FSCAL3     0xEA
#define SMARTRF_SETTING_FSCAL2     0x2A
#define SMARTRF_SETTING_FSCAL1     0x00
#define SMARTRF_SETTING_FSCAL0     0x1F
#define SMARTRF_SETTING_TEST0      0x09

#endif
