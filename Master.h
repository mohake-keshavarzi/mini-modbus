#ifndef SLAVE_H
#define SLAVE_H

#include "FunctionCodes.h"
#include "ModbusRequestCreator.h"
#include "ModbusRequestResponseParser.h"

class master {
private:
    Stream& serial;
    byte message[256];
    
    unsigned int timeout{300};
    unsigned int numberOfTries{2};
    unsigned int delayTime{100};
    byte exceptionCode{};

    word* registersBuffer{nullptr};
    boolean digitalsBuffer{nullptr};
    
    ModbusRequestResponseParser parser { nullptr };

    boolean  messageIsInvalid(ModbusRequestResponseParser parser, byte myID, byte expectedFunctionCode);

public:
    master(Stream& s);


    boolean writeSingleCoil(byte slaveID,word address,boolean value); // return false if process wasn't successfull or exception recieved
    boolean writeSingleRegister(byte slaveID,word address,word value);

    boolean readInputRegisters(byte slaveID,word startAddress,word quantity);

    byte getExceptionCode(){return exceptionCode;}
    ~master();
};


#endif