/*
 * File:   BME280.c
 * Author: Anac
 *
 * Created on 10 November 2020, 7:03 PM
 */

#include "BME280.h"

unsigned int i2cReadWord(i2c1_address_t addr, uint8_t reg)
{
    unsigned int var = i2c_read1ByteRegister(addr, reg+1);
    var <<= 8;
    var |= i2c_read1ByteRegister(addr, reg);
    
    return var;
}

void BME280Initialize(void)
{
	
			BME280Reset();
			BME280SetCtrlHum(0x01);
			BME280SetCtrlMeas(0x01, 0x01, 0x01);
			BME280SetConfig(0x03, 0x00);
            BME280GetCalibrateData();
            BME280GetTemperature();
}

void BME280SetCtrlMeas(const unsigned char temperatureOversampling, const unsigned char pressureOversampling, const unsigned char mode)
{
    int ctrlMeas = 0;
	ctrlMeas = (temperatureOversampling << 5) | (pressureOversampling << 2) | mode;	//Build the configuration bits
    
	 i2c_write1ByteRegister(BMEaddr, 0xF4, ctrlMeas);
}

void BME280SetConfig(const unsigned char standBy, const unsigned char filter)
{
	int config = 0;
	config = (standBy << 5) | (filter << 2);	//Build the configuration bits

	i2c_write1ByteRegister(BMEaddr, 0xF5, config);
}

void BME280SetCtrlHum(const int humididtyOversampling)
{
    i2c_write1ByteRegister(BMEaddr, 0xF2, humididtyOversampling);
}

bool BME280StatusMeasuringBusy(void)
{
    int var = BME280GetStatus() & 8;

	if (var == 8)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int BME280GetStatus(void)
{
    return i2c_read1ByteRegister(BMEaddr, 0xF3);
}

void BME280Reset(void)
{
    i2c_write1ByteRegister(BMEaddr, 0xE0, 0xB6);
}

void BME280GetCalibrateData(void)
{
    dig_T1 = i2cReadWord(BMEaddr, 0x88);
    dig_T2 = i2cReadWord(BMEaddr, 0x8A);
    dig_T3 = i2cReadWord(BMEaddr, 0x8C);
    
    dig_P1 = i2cReadWord(BMEaddr, 0x8E);
    dig_P2 = i2cReadWord(BMEaddr, 0x90);
    dig_P3 = i2cReadWord(BMEaddr, 0x92);
    dig_P4 = i2cReadWord(BMEaddr, 0x94);
    dig_P5 = i2cReadWord(BMEaddr, 0x96);
    dig_P6 = i2cReadWord(BMEaddr, 0x98);
    dig_P7 = i2cReadWord(BMEaddr, 0x9A);
    dig_P8 = i2cReadWord(BMEaddr, 0x9C);
    dig_P9 = i2cReadWord(BMEaddr, 0x9E);

    dig_H1 = i2c_read1ByteRegister(BMEaddr, 0xA1);
    dig_H2 = i2cReadWord(BMEaddr, 0xE1);
    dig_H3 = i2c_read1ByteRegister(BMEaddr, 0xE3);
    dig_H4 = i2c_read1ByteRegister(BMEaddr, 0xE4);
    unsigned int nibbleTemp = i2c_read1ByteRegister(BMEaddr, 0xE5);
    dig_H5 = i2c_read1ByteRegister(BMEaddr, 0xE6);
    dig_H6 = i2c_read1ByteRegister(BMEaddr, 0xE7);
    
    dig_H4 <<= 4;
    dig_H4 |= nibbleTemp & 0x0F;
    
    dig_H5 <<= 4;
    dig_H5 |= (nibbleTemp & 0xF0) >> 4;
    
    
    
    
}

float BME280GetTemperature(void)
{
    BME280SetCtrlMeas(0x01, 0x01, 0x01);
    while (BME280StatusMeasuringBusy())
    {
    }
    
    signed long int adc_T = i2c_read2ByteRegister(BMEaddr, 0xFA);
    adc_T <<= 8; // move in XLSB register
    adc_T |= i2c_read1ByteRegister(BMEaddr, 0xFC);
    adc_T >>= 4; // Only uses top 4 bits of XLSB register 
    
    int32_t var1;
    int32_t var2;
    int32_t temperatureT;
    int32_t temperature_min = -4000;
    int32_t temperature_max = 8500;

    var1 = (int32_t)((adc_T / 8) - ((int32_t)dig_T1 * 2));
    var1 = (var1 * ((int32_t)dig_T2)) / 2048;
    var2 = (int32_t)((adc_T / 16) - ((int32_t)dig_T1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)dig_T3)) / 16384;
    t_fine = var1 + var2;
    temperatureT = (t_fine * 5 + 128) / 256;

    if (temperatureT < temperature_min)
    {
        temperatureT = temperature_min;
    }
    else if (temperatureT > temperature_max)
    {
        temperatureT = temperature_max;
    }
    
    float tempT = temperatureT;
    
    return tempT/100;
    
}

float BME280GetPressure(void)
{    
    BME280SetCtrlMeas(0x01, 0x01, 0x01);
    while (BME280StatusMeasuringBusy())
    {
    }
        
    unsigned long int adc_P = i2c_read2ByteRegister(BMEaddr, 0xF7);
    adc_P <<= 8; // move in XLSB register
    adc_P |= i2c_read1ByteRegister(BMEaddr, 0xF9);
    adc_P >>= 4; // Only uses top 4 bits of XLSB register
    
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    uint32_t var5;
    uint32_t pressure;
    uint32_t pressure_min = 30000;
    uint32_t pressure_max = 110000;

    var1 = (((int32_t)t_fine) / 2) - (int32_t)64000;
    var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t)dig_P6);
    var2 = var2 + ((var1 * ((int32_t)dig_P5)) * 2);
    var2 = (var2 / 4) + (((int32_t)dig_P4) * 65536);
    var3 = (dig_P3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8;
    var4 = (((int32_t)dig_P2) * var1) / 2;
    var1 = (var3 + var4) / 262144;
    var1 = (((32768 + var1)) * ((int32_t)dig_P1)) / 32768;

    /* avoid exception caused by division by zero */
    if (var1)
    {
        var5 = (uint32_t)((uint32_t)1048576) - adc_P;
        pressure = ((uint32_t)(var5 - (uint32_t)(var2 / 4096))) * 3125;

        if (pressure < 0x80000000)
        {
            pressure = (pressure << 1) / ((uint32_t)var1);
        }
        else
        {
            pressure = (pressure / (uint32_t)var1) * 2;
        }

        var1 = (((int32_t)dig_P9) * ((int32_t)(((pressure / 8) * (pressure / 8)) / 8192))) / 4096;
        var2 = (((int32_t)(pressure / 4)) * ((int32_t)dig_P8)) / 8192;
        pressure = (uint32_t)((int32_t)pressure + ((var1 + var2 + dig_P7) / 16));

        if (pressure < pressure_min)
        {
            pressure = pressure_min;
        }
        else if (pressure > pressure_max)
        {
            pressure = pressure_max;
        }
    }
    else
    {
        pressure = pressure_min;
    }
    
    float tempp = pressure;

    return tempp/100;
     
}

int BME280GetHumidity(void)
{

    BME280SetCtrlMeas(0x01, 0x01, 0x01);
    
    while (BME280StatusMeasuringBusy())
    {
    }   
    
    unsigned long int adc_H = i2c_read2ByteRegister(BMEaddr, 0xFD);
    
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    uint32_t humidity;
    uint32_t humidity_max = 102400;

    var1 = t_fine - ((int32_t)76800);
    var2 = (int32_t)(adc_H * 16384);
    var3 = (int32_t)(((int32_t)dig_H4) * 1048576);
    var4 = ((int32_t)dig_H5) * var1;
    var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
    var2 = (var1 * ((int32_t)dig_H6)) / 1024;
    var3 = (var1 * ((int32_t)dig_H3)) / 2048;
    var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    var2 = ((var4 * ((int32_t)dig_H2)) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * ((int32_t)dig_H1)) / 16);
    var5 = (var5 < 0 ? 0 : var5);
    var5 = (var5 > 419430400 ? 419430400 : var5);
    humidity = (uint32_t)(var5 / 4096);    
    
    if (humidity > humidity_max)
    {
        humidity = humidity_max;
    }

    return humidity / 1024;
    //float hTemp = humidity;
    
    //return hTemp / 1024;
}