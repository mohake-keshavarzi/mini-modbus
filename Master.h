#ifndef MASTER_H
#define MASTER_H

#include "FunctionCodes.h"
#include "ModbusRequestCreator.h"
#include "ModbusRequestResponseParser.h"

class Master {
private:
    Stream& serial;
    byte message[256];
    
    unsigned int timeout{300};
    unsigned int numberOfTries{2};
    unsigned int delayTime{100};
    byte exceptionCode{};

    word* registersBuffer;
    boolean* digitalsBuffer;
    
    ModbusRequestResponseParser parser { nullptr };

    boolean  messageIsInvalid(ModbusRequestResponseParser &parser, byte myID, byte expectedFunctionCode);
    boolean isQuantityInRange(int quantity, int memoryType);
public:
    Master(Stream& s);


    boolean writeSingleCoil(byte slaveID,word address,boolean value); // return false if process wasn't successfull or exception recieved 
    boolean writeSingleRegister(byte slaveID,word address,word value);

    boolean readInputRegisters(byte slaveID,word startAddress,word quantity); // return false if process wasn't successfull or exception recieved or quantity is out of parsers potential capacity
    boolean readHoldingRegisters(byte slaveID,word startAddress,word quantity);
    boolean readCoils(byte slaveID,word startAddress,word quantity);
    boolean readDiscreteInputs(byte slaveID,word startAddress,word quantity);

    boolean writeCoils(byte slaveID,word startAddress,boolean* values, word quantity);
    boolean writeHoldingRegisters(byte slaveID,word startAddress,word* values, word quantity);
    

    byte getExceptionCode(){return exceptionCode;}
    byte* getResponseMessage(){return message;}

    boolean* getDigitalsBuffer(){return digitalsBuffer;}
    word* getRegistersBuffer(){return registersBuffer;}
    ~Master();
};


#endif