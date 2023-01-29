#include "ModbusRequestCreator.h"
#include "FunctionCodes.h"
#include "WordFunctions.h"


ModbusRequestCreator::ModbusRequestCreator(uint8_t slaveID){
    this->slaveID=slaveID;

    
}

uint16_t ModbusRequestCreator::createReadCoilsRequest(uint16_t startAddress,uint16_t numberOfCoils){
    // size is slaveID + function code + startAddress + numOfCoils = 1+1+2+2=6
    message[0]=slaveID;
    message[1] = READ_COILS_FUNCTIONCODE;
    message[2]=funcs.getMSByte(startAddress);
    message[3]=funcs.getLSByte(startAddress);
    message[4]=funcs.getMSByte(numberOfCoils);
    message[5]=funcs.getLSByte(numberOfCoils);

    return 6;
}

uint16_t ModbusRequestCreator::createWriteSingleRegisterRequest(uint16_t address,word value){
    // size is slaveID + function code + address + value = 1+1+2+2=6
    message[0]=slaveID;
    message[1] = WRITE_SINGLE_REGISTER_FUNCTIONCODE;
    message[2]=funcs.getMSByte(address);
    message[3]=funcs.getLSByte(address);
    message[4]=funcs.getMSByte(value);
    message[5]=funcs.getLSByte(value);

    return 6;
}

uint16_t ModbusRequestCreator::createReadInputRegistersRequest(uint16_t startAddress,uint16_t numOfRegisters){
    // size is slaveID + function code + startAddress + numOfRegisters = 1+1+2+2=6
    message[0]=slaveID;
    message[1] = READ_INPUT_REGISTERS_FUNCTIONCODE;
    message[2]=funcs.getMSByte(startAddress);
    message[3]=funcs.getLSByte(startAddress);
    message[4]=funcs.getMSByte(numOfRegisters);
    message[5]=funcs.getLSByte(numOfRegisters);
    
    return 6;
}

uint16_t ModbusRequestCreator::createWriteSingleCoilRequest(uint16_t address,boolean value){

    // size is slaveID + function code + address + value = 1+1+2+2=6
    message[0]=slaveID;
    message[1] = WRITE_SINGLE_COIL_FUNCTIONCODE;
    message[2]= funcs.getMSByte(address);
    message[3]= funcs.getLSByte(address);
    if(value){
        message[4]=0xFF;
        message[5]=0x00;
    }else{
        message[4]=0x00;
        message[5]=0x00;
    }

    return 6;
}

ModbusRequestCreator::~ModbusRequestCreator()
{}
