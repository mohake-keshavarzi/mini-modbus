#ifndef MODBUS_FUNCTION_CODES_H
#define MODBUS_FUNCTION_CODES_H

#define MASTER_ID 0x00

#define COILS 1
#define HOLDING_REGISTERS 2
#define INPUT_REGISTERS 3

#define WRITE_SINGLE_COIL_FUNCTIONCODE 0x05
#define WRITE_SINGLE_REGISTER_FUNCTIONCODE 0x06
#define READ_COILS_FUNCTIONCODE 0x01
#define READ_INPUT_REGISTERS_FUNCTIONCODE 0x04

#define INVALID_FUNCTION_CODE_EXCEPTION_CODE 0x01
#define INVALID_DATA_ADDRESS_EXCEPTION_CODE 0x02
#define INVALID_DATA_VALUE_EXCEPTION_CODE 0x03
#define EXECUTION_PROBLEM_EXCEPTION_CODE 0x04


#endif