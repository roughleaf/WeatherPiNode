#ifndef NRF24L01
#define	NRF24L01

#include <xc.h> // include processor files - each processor file is guarded.  
#include "../mcc_generated_files/mcc.h"
#include "../mcc_generated_files/spi2.h"
#include "../mcc_generated_files/drivers/spi_master.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CONFIG_REG 0x00
#define EN_AA_REG 0x01
#define EN_RXADDR_REG 0x02
#define SETUP_AW_REG 0x03
#define SETUP_RETR_REG 0x04
#define RF_CH_REG 0x05
#define RF_SETUP_REG 0x06
#define STATUS_REG 0x07
#define OBSERVE_TX_REG 0x08
#define CD_REG 0x09				// CD = Carrier Detect
#define RX_ADDR_P0_REG 0x0A
#define RX_ADDR_P1_REG 0x0B
#define RX_ADDR_P2_REG 0x0C
#define RX_ADDR_P3_REG 0x0D
#define RX_ADDR_P4_REG 0x0E
#define RX_ADDR_P5_REG 0x0F
#define TX_ADDR_REG 0x10
#define RX_PW_P0_REG 0x11
#define RX_PW_P1_REG 0x12
#define RX_PW_P2_REG 0x13
#define RX_PW_P3_REG 0x14
#define RX_PW_P4_REG 0x15
#define RX_PW_P5_REG 0x16
#define FIFO_STATUS_REG 0x17
#define DYNPD_REG 0x1C
#define FEATURE_REG 0x1D

#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xA0
#define ACTIVATE 0x50
#define R_RX_PL_WID 0x60

#define NRF24_CE PORTCbits.RC3
#define NRF24_CSN PORTCbits.RC4
#define NRF24_IRQ PORTAbits.RA6

unsigned char NRFChannel = 0;

char NRFAddr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};

void NRF24GetChanel(void);
void NRF24Initialize(void);
unsigned char NRF24ReadRegister(unsigned char reg);
void NRF24ReadRegisterBytes(unsigned char reg, char* buff, unsigned int len);
void NRF24WriteRegister(unsigned char reg, unsigned char value);
void NRF24WriteRegisterBytes(unsigned char reg, char* data, unsigned int len);
void NRF24ReadPayload(char* buff, unsigned int len);
int NRF24TransmitToChannel(const char* txBuff, unsigned int length);
unsigned char NRF24ReadStatus(void);
void NRF24PTXmode(void);
void NRF24PRXmode(void);
void NRF24FlushTX(void);
void NRF24FlushRX(void);
void Activate(void);
unsigned int GetRXWidth(void);
void NRF24PowerOn(void);
void NRF24PowerOff(void);

#endif	/* XC_HEADER_TEMPLATE_H */

