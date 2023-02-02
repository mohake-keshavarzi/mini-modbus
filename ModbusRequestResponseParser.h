#ifndef MODBUS_REQUEST_RESPONSE_PARSER_H
#define MODBUS_REQUEST_RESPONSE_PARSER_H

#include "Arduino.h"
#include "WordFunctions.h"

class ModbusRequestResponseParser
{
private:
    byte *message;
    WordFunctions funcs{};
public:
    ModbusRequestResponseParser(uint8_t ID, byte *message);
    void setMessage(byte *message){this->message=message;};
    uint8_t getSlaveID();
    uint8_t getFunctionCode();
    uint16_t getAddress();
    uint16_t getNumberOfRegsOrCoils();
    uint8_t getByteCount();
    uint16_t getWriteSingleRegisterValue();
    uint8_t getWriteSingleCoilValue();
    uint16_t getInputRegistersValues(uint32_t index);

    boolean isItException();
    uint8_t getExceptionCode();

    ~ModbusRequestResponseParser();
};


#endif