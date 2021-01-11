// TODO
// Change RTC interrupt to trigger every minute for deployment. It's set for every second for debugging
// Save data that were unable to be sent to RAM to be sent later. Create Circular Buffer for this
// Refactor to move functions to another folder and shorten the main function. This will improve readability


#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/drivers/i2c_simple_master.h"
#include "mcc_generated_files/drivers/spi_master.h"
#include "drivers/i2cLCD.h"
#include "drivers/DS1337.h"
#include "drivers/DS18B20.h"
#include "drivers/BME280.h"
#include "drivers/NRF24L01.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
//#include "drivers/i2cLCD.c"

#define _XTAL_FREQ 64000000

// ========================== RTC Variables ========================
uint8_t secs = 0;
uint8_t tenSecs = 0;
uint8_t min = 0;
uint8_t tenMin = 0;
uint8_t hour = 0;
uint8_t tenHour = 0;
uint8_t day = 0;
uint8_t tenDay = 0;
uint8_t month = 0;
uint8_t tenMonth = 0;
uint8_t year = 0;
uint8_t tenYear = 0;
uint8_t rtcAlarm = 0;
// =================================================================

// ================ Interrupt Function Prototypes ==================
void rainCounterISR(void);
void minuteTriggerISR(void);
void tenMinuteTriggerISR(void);
void nrf24ISR(void);
// =================================================================
// ================== General Function Prototypes ==================
void TransmitSensorData(void);
void RequestDateTimeUpdate(void);
void UpdateRtcVariables(void);
// =================================================================

long rainCount = 0;

bool minuteTriggerFlag = false;     // Use flags to handle interrupts
bool tenMinuteTriggerFlag = false;  // Use flags to handle interrupts
bool nrfIrqTriggerFlag = false;

bool DateTimeSet = false;

void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();

    // Set up interrupt Handlers
    INT0_SetInterruptHandler(rainCounterISR);
    INT1_SetInterruptHandler(minuteTriggerISR);
    INT2_SetInterruptHandler(tenMinuteTriggerISR);
    IOCAF6_SetInterruptHandler(nrf24ISR);
    
    // Initialize attached devices and interrupts
    //LCD_Innitialize();   // LCD used for initial development and debugging.
    BME280Initialize();
    INTERRUPT_Initialize();
    RTC_Innitialize();
    NRF24Initialize();
    
    RTCEnableMinuteInterrupt();   // Need to change this to every minute

    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    PORTCbits.RC7 = !DateTimeSet;
    
    __delay_ms(2000);      // Needed for NRF24L01+ to be stable
    RequestDateTimeUpdate();    
    while (1)       // Main While Loop
    {
        if (nrfIrqTriggerFlag) // If interrupt was triggered on NRF24L01
        {
            nrfIrqTriggerFlag = false;            
            unsigned char NrfStatus = NRF24ReadStatus();

            if (NrfStatus & 0x40) // RX Received
            {
                char rxBuff[32] = {0};
                unsigned int rxWidth = GetRXWidth();
                NRF24ReadPayload(rxBuff, rxWidth);
                RTCSetDateTime(rxBuff);
                DateTimeSet = true;

                NRF24WriteRegister(STATUS_REG, (NrfStatus | 0x40));
                NRF24FlushRX();

                if (!DateTimeSet)
                {
                    NRF24PRXmode(); // Remove for release build
                    __delay_us(200); // Remove for release build
                    NRF24_CE = 1; // Remove for release build   
                }
                else
                {
                    NRF24PowerOff();
                }
            }
            //==============================================================================
            // Todo
            // Parts of below needs to be implemented in this section
            if (NrfStatus & 0x20) // TX sent
            {
                NRF24WriteRegister(STATUS_REG, (NrfStatus | 0x20));
                NRF24FlushTX();

                if (!DateTimeSet)
                {
                    NRF24PRXmode(); // Remove for release build
                    __delay_us(200); // Remove for release build
                    NRF24_CE = 1; // Remove for release build   
                }
                else
                {
                    NRF24PowerOff();
                }
            }
            //==================================================================================
            // Todo
            // Save data to transmit to so variables in stack.
            // Calculate how much memory I will need in order to decide
            // how many datasets to save before recorded data is lost.
            // Then I need to transmit all the datasets once connection to 
            // WeatherPi node is restored

            if (NrfStatus & 0x10) // Resend
            {
                NRF24WriteRegister(STATUS_REG, (NrfStatus | 0x10));
                NRF24FlushTX();
                if (!DateTimeSet)
                {
                    NRF24PRXmode();
                    __delay_us(200);
                    NRF24_CE = 1;
                }
                else
                {
                    NRF24PowerOff();
                }
            }

            NRF24WriteRegister(0x07, (NrfStatus | 0x70)); // Clear all interrupts
            PORTCbits.RC7 = !DateTimeSet;
        }
        
        if (minuteTriggerFlag)
        {        
            minuteTriggerFlag = false;
            RTCClearAlarmFlag();  
            PORTCbits.RC7 = !DateTimeSet;
            rtcAlarm = RTCReadAlarmFlag() & 0x0F;   // For future use   
            if (!DateTimeSet)
            {
                RequestDateTimeUpdate();
            }
        }
        
        if(tenMinuteTriggerFlag)
        {
            tenMinuteTriggerFlag = false;
            if(DateTimeSet)
            {
                TransmitSensorData();
                rainCount = 0;
            }
        }
    }
    SLEEP();
}

void rainCounterISR(void)
{
   //INTERRUPT_GlobalInterruptDisable();
   INTERRUPT_PeripheralInterruptDisable();
    rainCount++;
    __delay_ms(10); // software debounce
    INTERRUPT_PeripheralInterruptEnable();
    //INTERRUPT_GlobalInterruptEnable();
}

void minuteTriggerISR(void)
{
    minuteTriggerFlag = true;
}

void tenMinuteTriggerISR(void)
{
    tenMinuteTriggerFlag = true;
}

void nrf24ISR(void)
{
    nrfIrqTriggerFlag = true;
}

void TransmitSensorData(void)
{
    typedef union
    {
        float floatNumber;
        long intNumber;
        uint8_t bytes[4];
    } FLOATINTUNION_t;

    FLOATINTUNION_t numberToByte;   // Use union get bytes to transmit
    
    UpdateRtcVariables();   // populate RTC variable with latest date and time
            
    char txBuff[31] = {0};
    
    float BME280Temperature = BME280GetTemperature();
    float BME280Pressure = BME280GetPressure();
    int BME280Humidity = BME280GetHumidity();   
    float DS18B20Temperature = OneWireTemp();
    
//============================= Build byte string ==============================
    txBuff[0] = 1;          // Indicate that we are transmitting sensor data
    txBuff[1] = NRFChannel; // ID for this sensor node
    
    // Date String in BCD
    txBuff[2] = tenYear;
    txBuff[3] = year;
    txBuff[4] = tenMonth;
    txBuff[5] = month;
    txBuff[6] = tenDay;
    txBuff[7] = day;
    
    // Time String in BCD
    txBuff[8] = tenHour;
    txBuff[9] = hour;
    txBuff[10] = tenMin;
    txBuff[11] = min;
    txBuff[12] = tenSecs;
    txBuff[13] = secs;
    
    numberToByte.floatNumber = BME280Temperature;   
    txBuff[14] = numberToByte.bytes[0];
    txBuff[15] = numberToByte.bytes[1];
    txBuff[16] = numberToByte.bytes[2];
    txBuff[17] = numberToByte.bytes[3];
    
    txBuff[18] = (char)BME280Humidity;
    
    numberToByte.floatNumber = BME280Pressure;   
    txBuff[19] = numberToByte.bytes[0];
    txBuff[20] = numberToByte.bytes[1];
    txBuff[21] = numberToByte.bytes[2];
    txBuff[22] = numberToByte.bytes[3];
    
    numberToByte.floatNumber = DS18B20Temperature;   
    txBuff[23] = numberToByte.bytes[0];
    txBuff[24] = numberToByte.bytes[1];
    txBuff[25] = numberToByte.bytes[2];
    txBuff[26] = numberToByte.bytes[3];
    
    numberToByte.intNumber = rainCount;   
    txBuff[27] = numberToByte.bytes[0];
    txBuff[28] = numberToByte.bytes[1];
    txBuff[29] = numberToByte.bytes[2];
    txBuff[30] = numberToByte.bytes[3];
    
    NRF24TransmitToChannel(txBuff, 31);
}

void RequestDateTimeUpdate(void)
{
    char txBuff[2] = {0};

    txBuff[0] = 0;          // Indicate that we are requesting DateTime Data for the RTC
    txBuff[1] = NRFChannel; // ID for this sensor node
    
    NRF24TransmitToChannel(txBuff, 2);
}

void UpdateRtcVariables(void)
{
    secs = RTCGetSeconds();
    tenSecs = (secs & 0xF0) >> 4;
    secs = secs & 0X0F;

    min = RTCGetMinutes();
    tenMin = (min & 0xF0) >> 4;
    min = min & 0X0F;

    hour = RTCGetHours();
    tenHour = (hour & 0xF0) >> 4;
    hour = hour & 0X0F;

    day = RTCGetDay();
    tenDay = (day & 0xF0) >> 4;
    day = day & 0X0F;

    month = RTCGetMonth();
    tenMonth = (month & 0xF0) >> 4;
    month = month & 0X0F;

    year = RTCGetYear();
    tenYear = (year & 0xF0) >> 4;
    year = year & 0X0F;
}
/**
 End of File
*/