#include "Slave.h"

Slave::Slave(uint16_t id):id(id)
{
    
}

void Slave::setCoilsRefrence(boolean* coilsArray, uint16_t size)
{
    responseCreator.setCoilsRefrence(coilsArray,size);
}

void Slave::setInputDiscretesRefrence(boolean* inputDiscretesArray,uint16_t size)
{
    responseCreator.setInputDiscretesRefrence(inputDiscretesArray,size);
}

void Slave::setHoldingRegistersRefrence(word* holdingRegistersArray,uint16_t size)
{
    responseCreator.setHoldingRegistersRefrence(holdingRegistersArray,size);
}

void Slave::setInputRegistersRefrence(word* inputRegistersArray,uint16_t size)
{
    responseCreator.setInputRegistersRefrence(inputRegistersArray,size);
}


Slave::~Slave()
{
}
