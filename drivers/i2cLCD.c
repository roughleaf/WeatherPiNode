/*
 * File:   i2cLCD.c
 * Author: Anac
 *
 * Created on 07 November 2020, 9:09 AM
 */


#include "i2cLCD.h"
#include "../mcc_generated_files/mcc.h"
#include "../mcc_generated_files/drivers/i2c_simple_master.h"
#include <xc.h>

void LCDWriteNibble(unsigned char nibble, unsigned char rs)
{
	nibble |= rs;
	nibble |= 0x08;		// Turn on backlight

    uint8_t data = nibble | 0x04;
    
	i2c_writeNBytes(0x27, &data, 1);
    data = nibble & 0xFB;
	i2c_writeNBytes(0x27, &data, 1);
	__delay_us(50);
}

void LCDWriteCommand(const unsigned char command)
{
	unsigned char rs = 0;

	LCDWriteNibble(command & 0xF0, rs);
	LCDWriteNibble((command << 4) & 0xF0, rs);
}

void LCDWriteCharacter(const unsigned char data)
{
	unsigned char rs = 1;

	LCDWriteNibble(data & 0xF0, rs);
	LCDWriteNibble((data << 4) & 0xF0, rs);
}

void LCD_Innitialize(void)
{
    i2c_writeNBytes(0x27,0x00, 1);
	__delay_ms(30);
	LCDWriteCommand(0x03);
	__delay_ms(5);
	LCDWriteCommand(0x03);
	__delay_ms(5);
	LCDWriteCommand(0x03);
	__delay_ms(5);
	LCDWriteCommand(0x02);
	__delay_ms(5);
	LCDWriteCommand(0x20 | (2 << 2));
	__delay_ms(50);
	LCDWriteCommand(0x0C);
	__delay_ms(50);
	LCDWriteCommand(0x01);
	__delay_ms(50);
	LCDWriteCommand(0x04 | 0x02);
	__delay_ms(50);
}
