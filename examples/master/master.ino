#include "MiniModbus.h"

#define CONTROLLER_ID 0x01

Master miniModbusMaster{Serial};
boolean taskIsDone{false};

void setup() {
//    Serial2.begin(19200);
    Serial.begin(19200);
    pinMode(13, OUTPUT);
//    pinMode(20, INPUT_PULLUP);

    
}
void loop() {
    boolean values[2]={false,true};
//    digitalWrite(13,values[1]);
    if(miniModbusMaster.writeSingleCoil(CONTROLLER_ID,0x0000,true)){
        digitalWrite(13,true);
    }
    else{
        digitalWrite(13,false);
    }
    delay(500);

    
}
