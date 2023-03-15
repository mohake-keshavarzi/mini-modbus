/// @file ModbusRequestCreator.h
/// @brief This header file is used for creating Request frames in modbus.

#ifndef MODBUS_REQUEST_CREATOR_H
#define MODBUS_REQUEST_CREATOR_H

#include "Arduino.h"
#include "WordFunctions.h"

/// @brief A class with necessery tools to make Request packages to send from Master to an specific Slave.
///
/// Each instance of this class is responible for one slave.
///
class ModbusRequestCreator
{
private:
    uint8_t slaveID=0;
    WordFunctions funcs{};
    byte* message;
public:
    /// @brief Creates base instance.
    /// @param slaveID ID of the slave that this instance is responsible to make request frames for.
    ModbusRequestCreator(uint8_t slaveID);
    uint16_t createReadCoilsRequest(uint16_t startAddress,uint16_t numberOfCoils);
    uint16_t createReadDiscreteInputsRequest(uint16_t startAddress,uint16_t numberOfCoils);
    uint16_t createWriteSingleRegisterRequest(uint16_t address,word value);
    uint16_t createReadInputRegistersRequest(uint16_t startAddress,uint16_t numOfRegisters);
    uint16_t createReadHoldingRegistersRequest(uint16_t startAddress,uint16_t numOfRegisters);
    uint16_t createWriteSingleCoilRequest(uint16_t address,boolean value);
    uint16_t createWriteCoilsRequest(uint16_t startAddress,boolean* values,uint16_t quantity);
    uint16_t createWriteRegistersRequest(uint16_t startAddress,word* values,uint16_t quantity);
    
    byte* getMessage(){return this->message;}
    ~ModbusRequestCreator();
};



#endif