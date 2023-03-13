#ifndef SLAVE_H
#define SLAVE_H

#include "ModbusResponseCreator.h"
#include "ModbusRequestResponseParser.h"
#include "FunctionCodes.h"

class Slave
{
private:
    uint16_t id;
    Stream &serial;
    byte message[256];

    ModbusResponseCreator responseCreator{};
    ModbusRequestResponseParser parser{nullptr};
    
public:
    Slave(uint16_t id,Stream &s);

    void setCoilsRefrence(boolean* coilsArray,uint16_t size);
    void setInputDiscretesRefrence(boolean* inputDiscretesArray,uint16_t size);
    void setHoldingRegistersRefrence(word* holdingRegistersArray,uint16_t size);
    void setInputRegistersRefrence(word* inputRegistersArray,uint16_t size);

    void execute();

    void setOnReadRequestFunction(void func(int type,word startingAddress ,byte* values, int quantity) );
    void setOnWriteRequestFunction(void func(int type,word startingAddress, byte* values, int quantity) );

    byte* getMessage(){return message;}

    ~Slave();
};

#endif