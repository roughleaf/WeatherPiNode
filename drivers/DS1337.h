
#ifndef DS1337_DRIVER
#define	DS1337_DRIVER

#include "../mcc_generated_files/mcc.h"
#include "../mcc_generated_files/i2c1_master.h"
#include "../mcc_generated_files/drivers/i2c_simple_master.h"  
#include <xc.h>

#define RTC_ADDR 0x68

void RTC_Innitialize(void);
void RTCSetSeconds(uint8_t tenSeconds, uint8_t seconds);
void RTCSetMinutes(uint8_t tenMinutes, uint8_t minutes);
void RTCSetHours(uint8_t tenHours, uint8_t hours);
void RTCSetDay(uint8_t tenDay, uint8_t day);
void RTCSetMonth(uint8_t tenMonth, uint8_t month);
void RTCSetYear(uint8_t tenYear, uint8_t year);
void RTCEnableMinuteInterrupt(void);
void RTCClearAlarmFlag(void);
uint8_t RTCReadAlarmFlag(void);
void RTCSetDateTime(char * dateTime);

uint8_t RTCGetSeconds(void);
uint8_t RTCGetMinutes(void);
uint8_t RTCGetHours(void);
uint8_t RTCGetDay(void);
uint8_t RTCGetMonth(void);
uint8_t RTCGetYear(void);

#endif	/* DS1337_DRIVER */

