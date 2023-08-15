# MiniModbus

It is a simplified version of the Modbus protocol, designed for Arduino. The primary goal of this library is to _create request and response messages_ and parse those messages. Using the Slave.h and Master.h classes handles the communications ,too.

## Important Note
This library uses [AceCRC](https://www.arduino.cc/reference/en/libraries/acecrc/) library, version 1.1.0, to handle CRC checking. Please assure to add this library to your enviroment.
[AceCRC Github page](https://github.com/bxparks/AceCRC)

## Supported Fuction Codes
For now, the following Function Codes are supported:
* READ COILS (0x01)
* READ DISCRETE INPUTS  (0x02)
* READ HOLDING REGISTERS  (0x03)
* READ INPUT REGISTERS (0x04)
* WRITE SINGLE COIL (0x05)
* WRITE SINGLE REGISTER (0x06)
* WRITE MULTIPLE COILS  (0x0f)
* WRITE MULTIPLE REGISTERS  (0x10)

## Frame Structure

The whole frame structure is so close to the original Modbus RTU but with more simplifications. 
* At the start of each frame, there is one byte for the destination's ID _(Master's ID is set to 0 by default)._

Structure of the frame:  
` ID of the reciever(1 byte) + Function code(1 bute)+ Data + CRC`

## Important Note
Please note that the values for PARSER_DIGITALS_BUFFER_SIZE and PARSER_REGISTERS_BUFFER_SIZE should be exactly identical in both Slave and Master devices.

It depends on the device that you are using to what values could be suitable for this two parameters. Larger values give you the opprtunity to send more data on a single message but it will absorb lots of dynamic memmory.


## Examples
### Master

```C++

#include "MiniModbus.h"

#define ACTUATOR_ID 0x02

#define PARSER_DIGITALS_BUFFER_SIZE 20
#define PARSER_REGISTERS_BUFFER_SIZE 20

Master miniModbusMaster{Serial2,PARSER_DIGITALS_BUFFER_SIZE,PARSER_REGISTERS_BUFFER_SIZE}; ///////////
boolean taskIsDone{false};
word values[10]={800,250,87,65,32,95,12,1,75,698};

void setup() {
    Serial2.begin(19200);
    Serial.begin(19200);

    miniModbusMaster.setup(19200);    
}
void loop() {
    int quantity=5;
    
    if(miniModbusMaster.writeHoldingRegisters(ACTUATOR_ID,0x0000,values,quantity)){
        Serial.println("Registers Wrote Successfully");
    }
    else{
        Serial.println("Writing registers failed");
    }
    if(miniModbusMaster.readHoldingRegisters(ACTUATOR_ID, 0x0000, quantity)){
    
        Serial.println("Read Seccessfully");
        Serial.print("DATA:");
        for(int i{0};i<quantity;i++){
          Serial.print("  ");
          Serial.print(miniModbusMaster.getRegistersBuffer()[i]);    
        }
        Serial.println();
        
    }
    else{
        Serial.print("Not seccessful: ");
        Serial.println(miniModbusMaster.getExceptionCode());
    }
    printInHex(miniModbusMaster.getResponseMessage(),20);
    delay(500);    
}

void printInHex(byte* message ,int length){
    for (int i = 0; i < length; i++) {
        Serial.print(message[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

```

### Slave

``` C++
#include "MiniModbus.h"

#define MY_ID 0x02
#define NUM_OF_HOLDING_REGS 10
#define NUM_OF_INPUT_REGS 10

#define PARSER_DIGITALS_BUFFER_SIZE 20
#define PARSER_REGISTERS_BUFFER_SIZE 20

word holdingRegisters[NUM_OF_HOLDING_REGS] {1,2,3,4,5,6,7,8,9,10};
word inputRegisters[NUM_OF_INPUT_REGS] {10,20,30,40,50,60,70,80,90,100};


Slave miniModbusSlave{MY_ID,Serial,PARSER_DIGITALS_BUFFER_SIZE,PARSER_REGISTERS_BUFFER_SIZE};

// Based on Uno
void setup()
{
    Serial.begin(19200);
    miniModbusSlave.setup(19200);
    pinMode(13, OUTPUT);
    miniModbusSlave.setHoldingRegistersRefrence(holdingRegisters,NUM_OF_HOLDING_REGS);
    miniModbusSlave.setInputRegistersRefrence(inputRegisters, NUM_OF_INPUT_REGS);
}

void loop()
{
    miniModbusSlave.execute();
}

```
