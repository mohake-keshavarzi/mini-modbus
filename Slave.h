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

    void (*onReadRegisterfunc)(int type,word startingAddress, int quantity) {nullptr};
    void (*onWriteRegisterfunc)(int type,word startingAddress ,word* values, int quantity) {nullptr};

    void (*onReadDigitalfunc)(int type,word startingAddress, int quantity) {nullptr};
    void (*onWriteDigitalfunc)(int type,word startingAddress ,boolean* values, int quantity) {nullptr};

public:
    Slave(uint16_t id,Stream &s);

    void setCoilsRefrence(boolean* coilsArray,uint16_t size);
    void setDiscreteInputsRefrence(boolean* inputDiscretesArray,uint16_t size);
    void setHoldingRegistersRefrence(word* holdingRegistersArray,uint16_t size);
    void setInputRegistersRefrence(word* inputRegistersArray,uint16_t size);

    void execute();

    void setOnReadRegisterRequestFunction(void (*func)(int type,word startingAddress, int quantity) );
    void setOnWriteRegisterRequestFunction(void (*func)(int type,word startingAddress, word* values, int quantity) );
    void setOnReadDigitalRequestFunction(void (*func)(int type,word startingAddress , int quantity) );
    void setOnWriteDigitalRequestFunction(void (*func)(int type,word startingAddress, boolean* values, int quantity) );


    byte* getMessage(){return message;}

    ~Slave();
};

#endif