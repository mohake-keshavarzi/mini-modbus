#include "MiniModbus.h"
#define CONTROLLER_ID 0x01
#define ACTUATOR_ID 0x02

Master miniModbusMaster{Serial2};
boolean taskIsDone{false};

void setup() {
    Serial2.begin(19200);
    Serial.begin(19200);
    pinMode(13, OUTPUT);
//    pinMode(20, INPUT_PULLUP);

    
}
void loop() {
    int value = analogRead(A0);
    Serial.print("A0:");
    Serial.println(value);

    if(miniModbusMaster.writeSingleRegister(ACTUATOR_ID,0x0001,value)){
        analogWrite(13,value);
        Serial.println("Seccessful");

    }
    else{
        analogWrite(13,0);
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
