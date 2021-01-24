/*
 * File:   DS1337.c
 * Author: Anac
 *
 * Created on 07 November 2020, 1:58 PM
 */


#include "DS1337.h"
//#include "../mcc_generated_files/drivers/i2c_simple_master.h"

void RTC_Innitialize(void)
{
    RTCSetSeconds(0, 0);
    RTCSetMinutes(0, 0);
    RTCSetHours(0, 0);
    RTCClearAlarmFlag();
}

void RTCSetSeconds(uint8_t tenSeconds, uint8_t seconds)
{
    uint8_t secTime = tenSeconds * 16 + seconds;
    i2c_write1ByteRegister(RTC_ADDR, 0x00, secTime);
}

void RTCSetMinutes(uint8_t tenMinutes, uint8_t minutes)
{
    uint8_t minTime = tenMinutes * 16 + minutes;
    i2c_write1ByteRegister(RTC_ADDR, 0x01, minTime);
}

void RTCSetHours(uint8_t tenHours, uint8_t hours)
{
    uint8_t hourTime = tenHours * 16 + hours;
    i2c_write1ByteRegister(RTC_ADDR, 0x02, hourTime);
}

void RTCSetDay(uint8_t tenDay, uint8_t day)
{
    uint8_t dayDate = tenDay * 16 + day;
    i2c_write1ByteRegister(RTC_ADDR, 0x04, dayDate);
}

void RTCSetMonth(uint8_t tenMonth, uint8_t month)
{
    uint8_t monthDate = tenMonth * 16 + month;
    i2c_write1ByteRegister(RTC_ADDR, 0x05, monthDate);
}

void RTCSetYear(uint8_t tenYear, uint8_t year)
{
    uint8_t yearDate = tenYear * 16 + year;
    i2c_write1ByteRegister(RTC_ADDR, 0x06, yearDate);
}

void RTCSetDateTime(char * dateTime)
{
    RTCSetYear(dateTime[1], dateTime[2]);
    RTCSetMonth(dateTime[3], dateTime[4]);
    RTCSetDay(dateTime[5], dateTime[6]);
    RTCSetHours(dateTime[7], dateTime[8]);
    RTCSetMinutes(dateTime[9], dateTime[10]);
    RTCSetSeconds(dateTime[11], dateTime[12]);
}

uint8_t RTCGetSeconds(void)
{
    return i2c_read1ByteRegister(RTC_ADDR, 0x00);
}

uint8_t RTCGetMinutes(void)
{
    return i2c_read1ByteRegister(RTC_ADDR, 0x01);
}

uint8_t RTCGetHours(void)
{
    return i2c_read1ByteRegister(RTC_ADDR, 0x02);
}

uint8_t RTCGetDay(void)
{
    return i2c_read1ByteRegister(RTC_ADDR, 0x04);
}

uint8_t RTCGetMonth(void)
{
    return i2c_read1ByteRegister(RTC_ADDR, 0x05);
}

uint8_t RTCGetYear(void)
{
    return i2c_read1ByteRegister(RTC_ADDR, 0x06);
}

// Undefined behaviour if NodeID > 9.
// If NodeID > 9 then it need to be converted to BCD
void RTCEnableMinuteInterrupt(uint8_t NodeID)
{
    i2c_write1ByteRegister(RTC_ADDR, 0x07, 0b10000000); // When enables trigger once per second
    //i2c_write1ByteRegister(RTC_ADDR, 0x07, NodeID);     // When enabled trigger once per minute, NodeID sets the second offset
    i2c_write1ByteRegister(RTC_ADDR, 0x08, 0b10000000);
    i2c_write1ByteRegister(RTC_ADDR, 0x09, 0b10000000);
    i2c_write1ByteRegister(RTC_ADDR, 0x0A, 0b10000000);
    i2c_write1ByteRegister(RTC_ADDR, 0x0E, 0b00000101);  
}

void RTCClearAlarmFlag(void)
{
    i2c_write1ByteRegister(RTC_ADDR, 0x0F, 0x00); //Clear Alarm
}

uint8_t RTCReadAlarmFlag(void)
{
    return i2c_read1ByteRegister(RTC_ADDR, 0x0F);
}
