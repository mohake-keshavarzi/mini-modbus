#include "MiniModbus.h"

#define MY_ID 0x01
#define NUM_OF_COILS 2


boolean coils[NUM_OF_COILS] {};


Slave miniModbusSlave{MY_ID,Serial2};

// ModbusRequestResponseParser parser(MY_ID
// Based on Uno
void setup()
{
    Serial.begin(19200);
    Serial2.begin(19200);
    pinMode(13, OUTPUT);

    miniModbusSlave.setCoilsRefrence(coils,NUM_OF_COILS);
    
}

void loop()
{
    miniModbusSlave.execute();
    Serial.print("Coil0:");
    Serial.println(coils[0]);
    Serial.print("Coil1:");
    Serial.println(coils[1]);
    
    digitalWrite(13,coils[0]);
    
   
}
