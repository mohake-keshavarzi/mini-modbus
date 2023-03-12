#ifndef MODBUS_RESPONSE_CREATOR_H
#define MODBUS_RESPONSE_CREATOR_H

#include "Arduino.h"
#include "WordFunctions.h"

class ModbusResponseCreator
{
private:
    boolean *coils=nullptr,*inputDiscretes=nullptr;
    word *holdingRegisters=nullptr,*inputRegisters=nullptr;
    uint32_t coilsSize=0,holdingRegsSize=0,inputRegsSize=0, inputDiscretesSize=0;;
    byte *message;
    uint16_t message_size;

    WordFunctions funcs{};
public:
    ModbusResponseCreator();

    void setCoilsRefrence(boolean* coils_refrence,uint16_t size);
    void setInputDiscretesRefrence(boolean* inputDiscretes_refrence, uint16_t size);
    void setHoldingRegistersRefrence(word* holdingRegisters_refrence,uint16_t size);
    void setInputRegistersRefrence(word* inputRegisters_refrence, uint16_t size);

    
    uint16_t createReadCoilsResponse(uint16_t startAddress,uint16_t numberOfCoils);
    uint16_t createWriteSingleRegisterResponse(uint16_t address,word value);
    uint16_t createReadInputRegistersResponse(uint16_t startAddress,uint16_t numOfRegisters);
    uint16_t createWriteSingleCoilResponse(uint16_t address,boolean value);

    uint16_t createExceptionResponse(uint8_t functionCode,uint8_t exceptionCode);

    boolean isDataAddressWrong(uint16_t startAddress,uint16_t numberOfCoilsOrRegisters,uint8_t typeOfMemory);


    byte *getMessage(){return message;}
    ~ModbusResponseCreator();
};


#endif