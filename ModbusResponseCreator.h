#ifndef MODBUS_RESPONSE_CREATOR_H
#define MODBUS_RESPONSE_CREATOR_H

#include "Arduino.h"
#include "WordFunctions.h"

class ModbusResponseCreator
{
private:
    boolean *coils;
    word *holdingRegisters,*inputRegisters;
    uint32_t coilsSize=0,holdingRegsSize=0,inputRegsSize=0;
    byte *message;
    uint16_t message_size;

    WordFunctions funcs{};
public:
    ModbusResponseCreator(boolean* coils_refrence,uint16_t coilsSize,
    word* holdingRegisters_refrence,uint16_t holdingSize,
    word* inputRegisters_refrence, uint16_t inputSize );
    
    uint16_t createReadCoilsResponse(uint16_t startAddress,uint16_t numberOfCoils);
    uint16_t createWriteSingleRegisterResponse(uint16_t address,word value);
    uint16_t createReadInputRegistersResponse(uint16_t startAddress,uint16_t numOfRegisters);
    uint16_t createWriteSingleCoilResponse(uint16_t address,boolean value);

    uint16_t createExceptionResponse(uint8_t functionCode,uint8_t exceptionCode);

    boolean checkDataAddress(uint16_t startAddress,uint16_t numberOfCoilsOrRegisters);

    byte *getMessage(){return message;}
    ~ModbusResponseCreator();
};


#endif