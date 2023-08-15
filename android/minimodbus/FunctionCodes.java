package ir.m.h.keshavarzi.minimodbus;

public class FunctionCodes {

    public static final byte MASTER_ID = 0x00;

//    public static final int COILS = 1;
//    public static final int HOLDING_REGISTERS = 2;
//    public static final int INPUT_REGISTERS = 3;

    public static enum MemoryType{
        COILS,HOLDING_REGISTERS,INPUT_REGISTERS,DISCRETE_INPUTS
    }

    public static final byte EXCEPTION_OFFSET=(byte) 0x80;


    public static final byte WRITE_SINGLE_COIL_FUNCTIONCODE = 0x05;
    public static final byte WRITE_SINGLE_REGISTER_FUNCTIONCODE = 0x06;
    public static final byte READ_COILS_FUNCTIONCODE = 0x01;
    public static final byte READ_INPUT_REGISTERS_FUNCTIONCODE = 0x04;
    public static final byte WRITE_MULTIPLE_REGISTERS_FUNCTIONCODE = 0x10; //*1
    public static final byte READ_DISCRETE_INPUTS_FUNCTIONCODE = 0x02; //*2
    public static final byte WRITE_MULTIPLE_COILS_FUNCTIONCODE = 0x0f;
    public static final byte READ_HOLDING_REGISTERS_FUNCTIONCODE = 0x03; //*1


    public static final byte INVALID_FUNCTION_CODE_EXCEPTION_CODE = 0x01;
    public static final byte INVALID_DATA_ADDRESS_EXCEPTION_CODE = 0x02;
    public static final byte INVALID_DATA_VALUE_EXCEPTION_CODE = 0x03;
    public static final byte EXECUTION_PROBLEM_EXCEPTION_CODE = 0x04;

    public static final byte COIL_VALUE_SET =(byte) 0xFF;
    public static final byte COIL_VALUE_RESET = 0x00;
    public static final byte COIL_VALUE_ERROR =(byte) 0x80;

    public static final int SERIAL_MESSAGE_BUFFER_SIZE = 256;

}
