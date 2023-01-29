#include "ModbusResponseCreator.h"
#include "FunctionCodes.h"
#include "WordFunctions.h"

ModbusResponseCreator::ModbusResponseCreator(boolean* coils_refrence,uint16_t coilsSize,
    word* holdingRegisters_refrence,uint16_t holdingSize,
    word* inputRegisters_refrence, uint16_t inputSize)
:coils(coils_refrence),coilsSize(coilsSize),
inputRegisters(inputRegisters_refrence),holdingRegsSize(holdingSize),
holdingRegisters(holdingRegisters_refrence),inputRegsSize(inputSize)
{
    message = new byte[8];
    message_size=8;
}

uint16_t ModbusResponseCreator::createReadCoilsResponse(uint16_t startAddress,uint16_t numberOfCoils)
{
    
    if(startAddress>coilsSize-1 || startAddress<0 || startAddress+numberOfCoils>coilsSize-1){
        return createExceptionResponse(READ_COILS_FUNCTIONCODE,INVALID_DATA_ADDRESS_EXCEPTION_CODE);
    }
    delete[] message;
    message_size=3; //Master id +function code + byte count
    
    uint8_t byteCount{numberOfCoils/8};
    if(numberOfCoils%8 != 0) byteCount++;
    
    message+=byteCount;
    message = new byte[message_size];

    message[0]= MASTER_ID;
    message[1]= READ_COILS_FUNCTIONCODE;
    message[2]=byteCount;
    int currentPlaceInMessage=3;
    byte myByte=0x00;
    int i=startAddress;
    int j=0;
    while(i<numberOfCoils+startAddress || j%8!=7)
    {
        myByte>>1;
        if(i<numberOfCoils+startAddress){
            if(coils[i]){
                myByte | 0x80;
            }else{
                myByte & 0x7F;
            }
        }
        if(j%8==7){
            message[currentPlaceInMessage]=myByte;
            currentPlaceInMessage++;
            myByte=0x00;
        }
    i++;
    j++;
    }
    return message_size;
}


uint16_t ModbusResponseCreator::createWriteSingleRegisterResponse(uint16_t address,word value)
{
    if(address<0 || address>holdingRegsSize-1){
        return createExceptionResponse(WRITE_SINGLE_REGISTER_FUNCTIONCODE,INVALID_DATA_ADDRESS_EXCEPTION_CODE);
    }
    holdingRegisters[address]=value;
    delete[] message;
    message_size=6; //master ID+function code + address + value = 1+2+2
    message= new byte[message_size];
    message[0]=MASTER_ID;
    message[1]=WRITE_SINGLE_REGISTER_FUNCTIONCODE;
    message[2]= funcs.getMSByte(address);
    message[3]= funcs.getLSByte(address);
    message[4]= funcs.getMSByte(value);
    message[5]= funcs.getLSByte(value);

    return message_size;    
}

uint16_t ModbusResponseCreator::createReadInputRegistersResponse(uint16_t startAddress,uint16_t numOfRegisters){
    
    if(startAddress>inputRegsSize-1 || startAddress<0 || startAddress+numOfRegisters>inputRegsSize-1){
        return createExceptionResponse(READ_COILS_FUNCTIONCODE,INVALID_DATA_ADDRESS_EXCEPTION_CODE);
    }
    delete[] message;
    message_size=3; //master id +function code + byte count
    
    uint8_t byteCount{numOfRegisters*2};

    message_size+=byteCount;
    message = new byte[message_size];
    message[0]= MASTER_ID;
    message[1]= READ_INPUT_REGISTERS_FUNCTIONCODE;
    message[2]= byteCount;
    
    int currentPlaceInMessage=3;
    for(int i{startAddress}; i<numOfRegisters+startAddress; i++){
        message[currentPlaceInMessage]= funcs.getMSByte(inputRegisters[i]);
        currentPlaceInMessage++;
        message[currentPlaceInMessage]= funcs.getLSByte(inputRegisters[i]);
        currentPlaceInMessage++; 
    }

    return message_size;
}

uint16_t ModbusResponseCreator::createWriteSingleCoilResponse(uint16_t address,boolean value)
{
    if(address<0 || address>coilsSize-1){
        return createExceptionResponse(WRITE_SINGLE_REGISTER_FUNCTIONCODE,INVALID_DATA_ADDRESS_EXCEPTION_CODE);
    }
    delete[] message;
    // size is master id + function code + address + value = 1+2+2=5
    message_size=6;
    message= new byte[message_size];

    message[0]= MASTER_ID;
    message[1] = WRITE_SINGLE_COIL_FUNCTIONCODE;
    message[2]= funcs.getMSByte(address);
    message[3]= funcs.getLSByte(address);
    if(value){
        coils[address]=1;
        message[4]=0xFF;
        message[5]=0x00;
    }else{
        coils[address]=0;
        message[4]=0x00;
        message[5]=0x00;
    }

    return message_size;
}

uint16_t ModbusResponseCreator::createExceptionResponse(uint8_t functionCode, uint8_t exceptionCode)
{
    delete[] message;
    // size is master id + function code for error + ExceptionCode = 1+1+1
    message_size=3;
    message= new byte[message_size];

    message[0]= MASTER_ID;
    message[1]= functionCode+0x80;
    message[2]=exceptionCode;
    return message_size;
}


ModbusResponseCreator::~ModbusResponseCreator()
{
    delete[] message;
}

