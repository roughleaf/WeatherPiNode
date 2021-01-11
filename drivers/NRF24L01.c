#include "NRF24L01.h"

void NRF24GetChanel(void)
{
    NRFChannel = PORTA & 0b00000111;
}

void NRF24Initialize(void)
{
    NRF24_CE = 0;                               // Ensure standby mode                       
    NRF24_CSN = 1;                              // Chip Select Not is active low.
    __delay_ms(200);                             // Ensure powerup time.
    
    NRF24GetChanel();

    NRF24WriteRegister(CONFIG_REG, 0x0F); // RX, TX & Max Retry interrupt enabled, PRX mode, Power up.
    NRF24WriteRegister(EN_AA_REG, 0x01); // Enable auto acknowledge for data pipes 0
    NRF24WriteRegister(EN_RXADDR_REG, 0x01); // Enable only data pipe RX addresses 0
    NRF24WriteRegister(SETUP_AW_REG, 0x03);
    NRF24WriteRegister(SETUP_RETR_REG, 0xF3);
    NRF24WriteRegister(RF_CH_REG, 0x50); // Set to 2480Mhz, outer edge of chanel 13 but still within legal limits
    NRF24WriteRegister(RF_SETUP_REG, 0x0F); // Set gain to minimum for testing
    NRF24WriteRegister(STATUS_REG, 0x70); // Clear all Interrupt Flags

    NRF24WriteRegisterBytes(RX_ADDR_P0_REG, NRFAddr, 5); // Setup Receive Datapipe Address
    NRF24WriteRegisterBytes(TX_ADDR_REG, NRFAddr, 5); // Set Transmit Datapipe Address  

    Activate(); // Enable some special features

    NRF24WriteRegister(DYNPD_REG, 0x01);
    NRF24WriteRegister(FEATURE_REG, 0x04);
    
    NRF24FlushRX();
    NRF24FlushTX();
    
    NRF24PRXmode();

    NRF24_CE = 1;       // Enable Receiver
}

unsigned char NRF24ReadRegister(unsigned char reg)
{
    char buff[2] = {reg, 0};
    
    NRF24_CSN = 0;
    SPI2_Open(0);
    SPI2_ExchangeBlock(buff, 2);
    NRF24_CSN = 1;
    SPI2_Close();
    
    return buff[1];
}

void NRF24ReadRegisterBytes(unsigned char reg, char* buff, unsigned int len)
{
    buff[0] = reg;
    
    NRF24_CSN = 0;
    SPI2_Open(0);
    SPI2_ExchangeBlock(buff, len+1); 
    NRF24_CSN = 1;
    SPI2_Close();
    
    for (int x = 1; x < len + 1; x++)
	{
		buff[x-1] = buff[x];	// remove the status bit
	}    
}

void NRF24WriteRegister(unsigned char reg, unsigned char value)
{
    char buff[2] = {reg | 0x20, value};
    
    NRF24_CSN = 0;
    SPI2_Open(0);
    SPI2_WriteBlock(buff, 2);
    NRF24_CSN = 1;
    SPI2_Close();
}

void NRF24WriteRegisterBytes(unsigned char reg, char* data, unsigned int len)
{
    reg |= 0x20;
    
    char buff[7];
	buff[0] = reg;
	for (int x = 1; x < len + 1; x++)
	{
		buff[x] = data[x - 1];
	}
    
    NRF24_CSN = 0;
    SPI2_Open(0);
    SPI2_WriteBlock(buff, len + 1);
    NRF24_CSN = 1;
    SPI2_Close();
}

void NRF24ReadPayload(char* buff, unsigned int len)
{
    buff[0] = R_RX_PAYLOAD;
    
    NRF24_CSN = 0;
    SPI2_Open(0);
    SPI2_ExchangeBlock(buff, len+1);
    NRF24_CSN = 1;
    SPI2_Close();
    
    for (int x = 1; x < len + 1; x++)
	{
		buff[x-1] = buff[x];	// remove the status bit
	}       
 
}

int NRF24TransmitToChannel(const char* txBuff, unsigned int length)
{
    NRF24_CE = 0;                   // Ensure standby mode
    NRF24PowerOn();
	char buff[33];
	buff[0] = W_TX_PAYLOAD;			// Add instruction byte
	for (int x = 1; x < length + 1; x++)
	{
		buff[x] = txBuff[x - 1];	// Add payload bytes after instruction byte
	}

	NRF24PTXmode();					// Set PTX mode
    
    NRF24_CSN = 0;
    SPI2_Open(0);
    SPI2_WriteBlock(buff, length + 1);
    NRF24_CSN = 1;
    SPI2_Close();

	NRF24_CE = 1;
	__delay_us(10);
	NRF24_CE = 0;						// Signal to start transmit

	return 0;
}

unsigned char NRF24ReadStatus(void)
{
    NRF24_CSN = 0;
    SPI2_Open(0);
    unsigned char status = SPI2_ExchangeByte(0xFF);
    NRF24_CSN = 1;
    SPI2_Close();
    return status;
}

void NRF24PTXmode(void)
{
    char conf = NRF24ReadRegister(CONFIG_REG);
	conf &= 0xFE;
	NRF24WriteRegister(CONFIG_REG, conf);
    __delay_us(200);
}
void NRF24PRXmode(void)
{
    char conf = NRF24ReadRegister(CONFIG_REG);
	conf |= 0x01;
	NRF24WriteRegister(CONFIG_REG, conf);
    __delay_us(200);
}

void NRF24FlushTX(void)
{
    NRF24_CSN = 0;
    SPI2_Open(0);
    SPI2_ExchangeByte(0xE1);
    NRF24_CSN = 1;
    SPI2_Close();
}

void NRF24FlushRX(void)
{
    NRF24_CSN = 0;
    SPI2_Open(0);
    SPI2_ExchangeByte(0xE2);
    NRF24_CSN = 1;
    SPI2_Close();
}

void Activate(void)
{
    char spiCommand[2] = {ACTIVATE, 0x73};
    
    NRF24_CSN = 0;
    SPI2_Open(0);
    SPI2_WriteBlock(spiCommand, 2);
    NRF24_CSN = 1;
    SPI2_Close();
}

unsigned int GetRXWidth(void)
{
    char rxWidth[2] = { R_RX_PL_WID, 0 };
    NRF24_CSN = 0;
    SPI2_Open(0);
	SPI2_ExchangeBlock(rxWidth, 2);
    NRF24_CSN = 1;
    SPI2_Close();
    
    return rxWidth[1];
}

void NRF24PowerOn(void)
{
    char config = NRF24ReadRegister(CONFIG_REG);
	NRF24WriteRegister(CONFIG_REG, (config | 0x02));
	__delay_ms(2);								//1.5ms delay while internal oscelator powers up
}

void NRF24PowerOff(void)
{
    char config = NRF24ReadRegister(CONFIG_REG);
	NRF24WriteRegister(CONFIG_REG, (config & 0xFD));
	__delay_ms(2);
}