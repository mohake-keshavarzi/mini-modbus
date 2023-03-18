#ifndef SLAVE_H
#define SLAVE_H

#include "FunctionCodes.h"
#include "ModbusRequestCreator.h"
#include "ModbusRequestResponseParser.h"

class master {
private:
    Stream& serial;
    byte message[256];

    ModbusRequestCreator requestCreator {};
    ModbusRequestResponseParser parser { nullptr };

public:
    master(/* args */);
    ~master();
};

master::master(/* args */)
{
}

master::~master()
{
}

#endif