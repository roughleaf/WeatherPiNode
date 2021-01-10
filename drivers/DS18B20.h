#ifndef DS18B20_H
#define DS18B20_H

#include "../mcc_generated_files/mcc.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xc.h>

#define BUSDIR LATBbits.LATB5 // Set direction: input or output
#define BUSOUT TRISBbits.TRISB5 // Set output: high or low 
#define BUSIN PORTBbits.RB5 // Read input 

float OneWireTemp(void); // Returns the temperature in celsius 
unsigned int OneWireReset(void); // Sends a reset pulse to the sensor 
void OneWireWriteBit(unsigned char); // write a single bit to the OneWire
unsigned char OneWireReadBit(void); // reads a single bit 
void OneWireWriteByte(unsigned char); // writes a byte 
unsigned char OneWireReadByte(void); // reads a byte 
unsigned char OneWireRead(void); // reads the current status of the bus
void OneWireHigh(void); // sets the bus high
void OneWireRelease(void); // releases the bus 

#endif // DS18B20_H