#include "MiniModbus.h"
#define CONTROLLER_ID 0x01

Master miniModbusMaster{Serial2};
boolean taskIsDone{false};

void setup() {
    Serial2.begin(19200);
    Serial.begin(19200);
    pinMode(13, OUTPUT);
//    pinMode(20, INPUT_PULLUP);

    
}
void loop() {
    boolean values[2]={false,true};
//    digitalWrite(13,values[1]);
    if(miniModbusMaster.writeSingleCoil(CONTROLLER_ID,0x0004,true)){
        digitalWrite(13,true);
        Serial.println("Seccessful");

    }
    else{
        digitalWrite(13,false);
        Serial.print("Not seccessful: ");
        Serial.println(miniModbusMaster.getExceptionCode());
    }
    printInHex(miniModbusMaster.getResponseMessage(),16);
    delay(500);

    
}

void printInHex(byte* message ,int length){
    for (int i = 0; i < length; i++) {
        Serial.print(message[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}
