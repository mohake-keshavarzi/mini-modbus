#include "MiniModbus.h"
#include <Adafruit_AHTX0.h>

#define MY_ID 0x02
#define NUM_OF_COILS 0
#define NUM_OF_HOLDING_REGS 0
#define NUM_OF_INPUT_REGS 2

Adafruit_AHTX0 aht;

byte message[256];

boolean coils[NUM_OF_COILS] {};
word holdingRegs[NUM_OF_HOLDING_REGS], inputRegs[NUM_OF_INPUT_REGS];

ModbusResponseCreator myResponse(coils, NUM_OF_COILS, holdingRegs, NUM_OF_HOLDING_REGS, inputRegs, NUM_OF_INPUT_REGS);

// ModbusRequestResponseParser parser(MY_ID
void setup()
{
    Serial2.begin(19200);
    Serial.begin(19200);
    if (!aht.begin()) {
        Serial.println("Could not find AHT? Check wiring");
        while (1)
            delay(10);
    }
    Serial.println("AHT10 or AHT20 found");
}

void loop()
{
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    inputRegs[0] = (int)temp.temperature;
    inputRegs[1] = (int)humidity.relative_humidity;
    int messageSize = Serial2.available();
    int responseSize = 0;
    if (messageSize > 0) {
        Serial.println("New message");
        Serial2.readBytes(message, messageSize);

        ModbusRequestResponseParser parser((uint8_t)MY_ID, message);
        if (parser.getSlaveID() == MY_ID) {
            switch (parser.getFunctionCode()) {
            case WRITE_SINGLE_COIL_FUNCTIONCODE:
                Serial.print("Requeste address is:");
                Serial.println(parser.getAddress());

                if (myResponse.isDataAddressWrong(parser.getAddress(), 0, COILS)) {
                    Serial.println("Address is wrong");
                    responseSize = myResponse.createExceptionResponse(WRITE_SINGLE_COIL_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
                } else {
                    uint8_t coil_value = parser.getWriteSingleCoilValue();
                    if (coil_value != COIL_VALUE_ERROR)
                        responseSize = myResponse.createWriteSingleCoilResponse(parser.getAddress(), coil_value == COIL_VALUE_SET);
                    else
                        responseSize = myResponse.createExceptionResponse(WRITE_SINGLE_COIL_FUNCTIONCODE, INVALID_DATA_VALUE_EXCEPTION_CODE);
                }
                break;
            case READ_INPUT_REGISTERS_FUNCTIONCODE:
                Serial.println("READ INPUT REGISTERS REQUEST");
                if (myResponse.isDataAddressWrong(parser.getAddress(), parser.getNumberOfRegsOrCoils(), INPUT_REGISTERS)) {
                    responseSize = myResponse.createExceptionResponse(READ_INPUT_REGISTERS_FUNCTIONCODE, INVALID_DATA_ADDRESS_EXCEPTION_CODE);
                } else {
                    responseSize = myResponse.createReadInputRegistersResponse(parser.getAddress(), parser.getNumberOfRegsOrCoils());
                }

                for (int i { 0 }; i < responseSize; i++) {
                    Serial.println((int)myResponse.getMessage()[i], HEX);
                }
                break;
            }
            Serial.print("TRY TO WRITE");
            Serial.println(responseSize);
            Serial2.write(myResponse.getMessage(), responseSize);
        }
    }
    Serial2.flush();
    messageSize = 0;
    delay(10);
}
