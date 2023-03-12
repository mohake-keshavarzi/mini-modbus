#ifndef SLAVE_H
#define SLAVE_H

#include "ModbusResponseCreator.h"
class Slave
{
private:
    uint16_t id;
    ModbusResponseCreator responseCreator{};
public:
    Slave(uint16_t id);

    void setCoilsRefrence(boolean* coilsArray,uint16_t size);
    void setInputDiscretesRefrence(boolean* inputDiscretesArray,uint16_t size);
    void setHoldingRegistersRefrence(word* holdingRegistersArray,uint16_t size);
    void setInputRegistersRefrence(word* inputRegistersArray,uint16_t size);

    ~Slave();
};

#endif