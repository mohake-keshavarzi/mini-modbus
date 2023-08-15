package ir.m.h.keshavarzi.minimodbus;


public class ModbusRequestResponseParser {
    private static final String TAG = ModbusRequestResponseParser.class.getSimpleName();
    private byte[] message;
    private final boolean[] digitalValues; // Digital value refers to both coil and Discrete input values
    private final short[] registerValues;
    private int message_size;

    public ModbusRequestResponseParser(byte[] message,int digitalValuesBufferSize,int registerValuesBufferSize)
    {
        this.message = message;
        if(message == null) this.message_size=0;
        else this.message_size = message.length;
        digitalValues = new boolean[digitalValuesBufferSize];
        registerValues = new short[registerValuesBufferSize];
    }

    public boolean isValidCRC(int message_size)
    {
        short predicted_crc = WordFunctions.CRC16.calculate(message,0,message_size-2);
        short given_crc = WordFunctions.MSBLSBJoin(message[message_size-1],message[message_size-2]); ////
//        String s="";
////        given_crc = (short) 0x9ddb;
//        s+= String.format("%02X ", WordFunctions.getMSByte(given_crc));
//        s+= String.format("%02X ", WordFunctions.getLSByte(given_crc));
//        Log.d(TAG,"The given CRC is:" + s);
//        s="";
//        s+= String.format("%02X ", WordFunctions.getMSByte(predicted_crc));
//        s+= String.format("%02X ", WordFunctions.getLSByte(predicted_crc));
//        Log.d(TAG,"The predicted CRC is:" + s);

        return predicted_crc == given_crc;
    }

    public byte getFunctionCode() {
        return message[1];
    }

    public boolean isItException() {
        return getFunctionCode() < 0;
        //Byte in Java is always a signed type so if we have a one in the first bit from left
        // the resulting number will be negative
    }

    public byte getExceptionCode() {
        if (isItException()) {
            return message[2];
        }
        return 0;
    }

    public byte getSlaveID() {
        return message[0];
    }

    public short getAddress() {
        return WordFunctions.MSBLSBJoin(message[2], message[3]);
    }

    public short getNumberOfRegsOrDigitals() {
        return WordFunctions.MSBLSBJoin(message[4], message[5]);
    }

    public byte getByteCountInResponse() {
        return message[2];
    }

    public byte getByteCountInRequest() {
        return message[6];
    }


    public short getWriteSingleRegisterValue() {
        return WordFunctions.MSBLSBJoin(message[4], message[5]);
    }

    /**
     * For coils to set you should send 0xFF00 and for reset you should send 0x0000
     * If none of those values aren't received it will return 0x80
     * If true is received it will return 0xFF
     * If false is received it will return 0x00
     */
    public byte getWriteSingleCoilValue() {
        if (message[4] == (byte) 0xFF && message[5] == (byte) 0x00) {
            return FunctionCodes.COIL_VALUE_SET;
        } else if (message[4] == (byte) 0x00 && message[5] == (byte) 0x00) {
            return FunctionCodes.COIL_VALUE_RESET;
        } else {
            return FunctionCodes.COIL_VALUE_ERROR;
        }
    }

    public short getRegisterValueByIndexInResponse(int index)
    {
        return WordFunctions.MSBLSBJoin(message[3 + 2 * index], message[4 + 2 * index]);
    }

    public short getRegisterValueByIndexInRequest(int index)
    {
        return WordFunctions.MSBLSBJoin(message[7 + 2 * index], message[8 + 2 * index]);
    }

    public boolean getDigitalValueByIndexInResponse(int index)
    {
        byte b = message[3 + index / 8];
        int result = (b >> (index % 8)) & 0x01;
        if(result == 0x01) return true;
        else return false;
    }

    public boolean getDigitalValueByIndexInRequest(int index)
    {
        byte b = message[7 + index / 8];
        int result = (b >> (index % 8)) & 0x01;
        if(result == 0x01) return true;
        else return false;
    }

    public boolean[] getDigitalValuesArray()
    {
        // delete[] digitalValues;
        int count = 0;
        switch (getFunctionCode()) {
            case FunctionCodes.READ_COILS_FUNCTIONCODE:
            case FunctionCodes.READ_DISCRETE_INPUTS_FUNCTIONCODE:
                count = getByteCountInResponse() * 8;
                // digitalValues = new boolean[count];
                for (int i = 0 ; i < count; i++)
                digitalValues[i] = getDigitalValueByIndexInResponse(i);
                break;
            case FunctionCodes.WRITE_MULTIPLE_COILS_FUNCTIONCODE:
                count = getByteCountInRequest() * 8;
                // digitalValues = new boolean[count];
                for (int i = 0 ; i < count; i++)
                digitalValues[i] = getDigitalValueByIndexInRequest(i);
                break;

            default:
                return null;
        }
        return digitalValues;
    }

    public short[] getRegisterValuesArray()
    {
        // delete[] registerValues;
        int count = 0;
        switch (getFunctionCode()) {
            case FunctionCodes.READ_HOLDING_REGISTERS_FUNCTIONCODE:
            case FunctionCodes.READ_INPUT_REGISTERS_FUNCTIONCODE:
                count = getByteCountInResponse() / 2;
                // registerValues = new word[count];
                for (int i = 0 ; i < count; i++)
                registerValues[i] = getRegisterValueByIndexInResponse(i);
                break;
            case FunctionCodes.WRITE_MULTIPLE_REGISTERS_FUNCTIONCODE:
                count = getByteCountInRequest() / 2;
                // registerValues = new word[count];
                for (int i =0 ; i < count; i++)
                registerValues[i] = getRegisterValueByIndexInRequest(i);
                break;

            default:
                return null;
        }
        return registerValues;
    }

    public void setMessage(byte[] message) {
        this.message = message;
        this.message_size = message.length;
    }

    public int getMessage_size() {
        return message_size;
    }
}

