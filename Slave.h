#ifndef SLAVE_H
#define SLAVE_H

#include "ModbusResponseCreator.h"
#include "ModbusRequestResponseParser.h"
#include "FunctionCodes.h"

class Slave
{
private:

    unsigned long baudRate{};
    unsigned long interCharDelay{}; 

    uint16_t id;
    Stream &serial;
    unsigned int delayTime{10};
    byte message[SERIAL_MESSAGE_BUFFER_SIZE];

    ModbusResponseCreator responseCreator{};
    ModbusRequestResponseParser parser;

    void (*onReadRegisterfunc)(int type,word startingAddress, int quantity) {nullptr};
    void (*onWriteRegisterfunc)(int type,word startingAddress ,word* values, int quantity) {nullptr};

    void (*onReadDigitalfunc)(int type,word startingAddress, int quantity) {nullptr};
    void (*onWriteDigitalfunc)(int type,word startingAddress ,boolean* values, int quantity) {nullptr};

    unsigned short readSerial(byte* buffer);

    bool newCommandRecieved{false};

public:
    Slave(uint16_t id,Stream &s,uint16_t digitalValuesBufferSize,uint16_t registerValuesBufferSize);
    
    void setup(int baudRate);

    void setInterCharDelay(unsigned int delay);

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
    void setDelayTime(unsigned int t){this->delayTime=t;}

    bool getNewCommandRecivedStatus(){
        bool result{newCommandRecieved};
        newCommandRecieved=false;
        return result;
    }
    ~Slave();
};

#endif