 #include "DS18B20.h"

float OneWireTemp()
{
    
    OneWireReset(); // Reset Pulse 
    OneWireWriteByte(0xCC); // Issue skip ROM command (CCh)
    OneWireWriteByte(0x44); // Convert T command (44h)
    __delay_ms(750); // maximum time to measure is 750ms according to datasheet
    //while(!BUSIN); // DS will hold line low while making measurement
    
    OneWireReset(); // Start new command sequence 
    OneWireWriteByte(0xCC); // Issue skip ROM command 
    OneWireWriteByte(0xBE); // Read Scratchpad (BEh) - 15 bits
    unsigned char LSB = OneWireReadByte();
    unsigned char MSB = OneWireReadByte();
    OneWireReset(); // Stop Reading 
    unsigned int data = MSB;
    float temperature = (data << 8) | LSB;
    return (temperature/16); 
   
}

void OneWireHigh()
{
    BUSDIR = 0; // Set as output
    BUSOUT = 1; // Set high 
}

void OneWireRelease()
{
    BUSDIR = 0; // Set as output
    BUSOUT = 0; // Set low 
}

unsigned char OneWireRead()
{
    return BUSIN;
}

unsigned int OneWireReset()
{
    OneWireRelease();
    __delay_us(240); // 480uS Delay
    __delay_us(240);
    OneWireHigh();
    __delay_us(70); // wait 70 uS before reading
    unsigned int OW = OneWireRead(); // check for OneWire
    __delay_us(205); // 410 uS delay
    __delay_us(205);
    OneWireHigh(); // give bus back to OneWire
    return OW; 
}


void OneWireWriteBit(unsigned char b)
{
    if(b)
    {
        OneWireRelease();
        __delay_us(6);
        OneWireHigh();
        __delay_us(64);
    }
    else
    {
        OneWireRelease();
        __delay_us(60);
        OneWireHigh();
        __delay_us(10);
    }
}

unsigned char OneWireReadBit()
{
    OneWireRelease();
    __delay_us(6); // wait 6uS
    OneWireHigh();
    __delay_us(9); // wait 9uS
    unsigned char out = OneWireRead();
    __delay_us(55); // wait 55uS
    return out;
}

void OneWireWriteByte(unsigned char b)
{
    for(int i = 0; i < 8; i++)
    {
        OneWireWriteBit(b & 0x01); // send LS bit first 
        b = b >> 1;
    }
}

unsigned char OneWireReadByte(void)
{
    unsigned char out; 
    for(int i = 0; i < 8; i++)
    { // read in LS bit first
        out = out >> 1; // get out ready for next bit
        if(OneWireReadBit() & 0x01) // if its a one 
            out = out | 0x80; // place a 1 
    }
    return out;
}