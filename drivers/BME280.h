#ifndef _BME280
#define	_BME280

#include "../mcc_generated_files/mcc.h"
#include "../mcc_generated_files/i2c1_master.h"
#include "../mcc_generated_files/drivers/i2c_simple_master.h"  
#include <xc.h>
#include <math.h>

#define BMEaddr 0x76

#define BME280_S32_t int32_t
#define BME280_U32_t uint32_t

//SensorRawData BME280resultRaw;
//SensorData BME280Result;

unsigned char humidityOversamplingValue = 0x01; // Default to 1X over sampling
unsigned char pressureOversamplingValue = 0x01; // Default to 1X over sampling
unsigned char temperatureOversamplingValue = 0x01; // Default to 1X over sampling
unsigned char sensorModeValue = 0x01; // Default to forced mode

signed long rawTemperature = 0;
unsigned long rawHumidity = 0;
unsigned long rawPressure = 0;

signed long int t_fine = 0;

static unsigned short int dig_T1 = 0;
static short int dig_T2 = 0;
static short int dig_T3 = 0;

static unsigned short int dig_P1 = 0;
static signed short int dig_P2 = 0;
static signed short int dig_P3 = 0;
static signed short int dig_P4 = 0;
static signed short int dig_P5 = 0;
static signed short int dig_P6 = 0;
static signed short int dig_P7 = 0;
static signed short int dig_P8 = 0;
static signed short int dig_P9 = 0;

static unsigned int dig_H1 = 0;
static signed long int dig_H2 = 0;
static unsigned int dig_H3 = 0;
static signed long int dig_H4 = 0;
static signed long int dig_H5 = 0;
static signed int dig_H6 = 0;


int BME280GetStatus(void);
void BME280GetCalibrateData(void);
BME280_S32_t BME280CompensateTemp(const BME280_U32_t adc_T);
BME280_U32_t BME280CompensatePressure(const BME280_S32_t adc_P);
BME280_U32_t BME280CompensateHumidity(const BME280_S32_t adc_H);

void BME280Initialize(void);
void BME280SetConfig(const unsigned char standBy, const unsigned char filter);
void BME280SetCtrlMeas(const unsigned char temperatureOversampling, const unsigned char pressureOversampling, const unsigned char mode);
void BME280SetCtrlHum(const int humididtyOversampling);
float BME280GetTemperature(void);
float BME280GetPressure(void);
int BME280GetHumidity(void);
bool BME280StatusMeasuringBusy(void);
void BME280Reset(void);
unsigned int i2cReadWord(i2c1_address_t addr, uint8_t reg);

#endif	/* XC_HEADER_TEMPLATE_H */

