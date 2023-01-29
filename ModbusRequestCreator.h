#ifndef MODBUS_REQUEST_CREATOR_H
#define MODBUS_REQUEST_CREATOR_H

#include "Arduino.h"
#include "WordFunctions.h"

class ModbusRequestCreator
{
private:
    uint8_t slaveID=0;
    WordFunctions funcs{};
    byte message[6];
public:
    ModbusRequestCreator(uint8_t slaveID);
    uint16_t createReadCoilsRequest(uint16_t startAddress,uint16_t numberOfCoils);
    uint16_t createWriteSingleRegisterRequest(uint16_t address,word value);
    uint16_t createReadInputRegistersRequest(uint16_t startAddress,uint16_t numOfRegisters);
    uint16_t createWriteSingleCoilRequest(uint16_t address,boolean value);
    byte* getMessage(){return this->message;}
    ~ModbusRequestCreator();
};



#endif