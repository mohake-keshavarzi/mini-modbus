#ifndef MODBUS_REQUEST_RESPONSE_PARSER_H
#define MODBUS_REQUEST_RESPONSE_PARSER_H

#include "Arduino.h"
#include "WordFunctions.h"

class ModbusRequestResponseParser
{
private:
    byte *message;
    WordFunctions funcs{};

    boolean* digitalValues;  //Digital value refers to both coil and Discrete input values
    word* registerValues;
public:
    ModbusRequestResponseParser(byte *message);
    void setMessage(byte *message){this->message=message;};
    uint8_t getSlaveID();
    uint8_t getFunctionCode();
    uint16_t getAddress();
    uint16_t getNumberOfRegsOrDigitals();
    uint8_t getByteCountInReponse();
    uint8_t getByteCountInRequest();
    uint16_t getWriteSingleRegisterValue();
    uint8_t getWriteSingleCoilValue();
    uint16_t getRegisterValueByIndexInResponse(uint32_t index);
    uint16_t getRegisterValueByIndexInRequest(uint32_t index);

    
    

    boolean getDigitalValueByIndexInResponse(uint32_t index);
    boolean getDigitalValueByIndexInRequest(uint32_t index);

    boolean* getDigitalValuesArray();
    word* getRegisterValuesArray();

    boolean isItException();
    uint8_t getExceptionCode();

    ~ModbusRequestResponseParser();
};


#endif