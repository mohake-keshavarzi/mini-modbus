package ir.m.h.keshavarzi.minimodbus;



public class ModbusRequestCreator {
    private byte[] message = new byte[8]; // increase message length to 8 to accommodate 16-bit values
    private final byte slaveID;

    public ModbusRequestCreator(byte slaveID) {
        this.slaveID = slaveID;
    }

    public short createReadCoilsRequest(short startAddress, short numberOfCoils) {
        // size is slaveID + function code + startAddress + numOfCoils + CRC = 1+1+2+2+2=8
        message = new byte[8];
        message[0] = slaveID;
        message[1] = FunctionCodes.READ_COILS_FUNCTIONCODE;
        message[2] = WordFunctions.getMSByte(startAddress);
        message[3] = WordFunctions.getLSByte(startAddress);
        message[4] = WordFunctions.getMSByte(numberOfCoils);
        message[5] = WordFunctions.getLSByte(numberOfCoils);
        short crc = WordFunctions.CRC16.calculate(message, 0, 6);
        message[6] = WordFunctions.getLSByte(crc);
        message[7] = WordFunctions.getMSByte(crc);
        return 8;
    }

    public short createReadDiscreteInputsRequest(short startAddress, short numberOfCoils) {
        // size is slaveID + function code + startAddress + numOfCoils + CRC = 1+1+2+2+2=8
        message = new byte[8];
        message[0] = slaveID;
        message[1] = FunctionCodes.READ_DISCRETE_INPUTS_FUNCTIONCODE;
        message[2] = WordFunctions.getMSByte(startAddress);
        message[3] = WordFunctions.getLSByte(startAddress);
        message[4] = WordFunctions.getMSByte(numberOfCoils);
        message[5] = WordFunctions.getLSByte(numberOfCoils);
        short crc = WordFunctions.CRC16.calculate(message, 0, 6);
        message[6] = WordFunctions.getLSByte(crc);
        message[7] = WordFunctions.getMSByte(crc);
        return 8;
    }

    public short createWriteSingleRegisterRequest(short address, short value) {
        // size is slaveID + function code + address + value + CRC = 1+1+2+2+2=8
        message = new byte[8];
        message[0] = slaveID;
        message[1] = FunctionCodes.WRITE_SINGLE_REGISTER_FUNCTIONCODE;
        message[2] = WordFunctions.getMSByte(address);
        message[3] = WordFunctions.getLSByte(address);
        message[4] = WordFunctions.getMSByte(value);
        message[5] = WordFunctions.getLSByte(value);
        short crc = WordFunctions.CRC16.calculate(message, 0, 6);
        message[6] = WordFunctions.getLSByte(crc);
        message[7] = WordFunctions.getMSByte(crc);
        return 8;
    }

    public short createReadInputRegistersRequest(short startAddress, short numOfRegisters) {
        // size is slaveID + function code + startAddress + numOfRegisters + CRC = 1+1+2+2+2=8
        message = new byte[8];
        message[0] = slaveID;
        message[1] = FunctionCodes.READ_INPUT_REGISTERS_FUNCTIONCODE;
        message[2] = WordFunctions.getMSByte(startAddress);
        message[3] = WordFunctions.getLSByte(startAddress);
        message[4] = WordFunctions.getMSByte(numOfRegisters);
        message[5] = WordFunctions.getLSByte(numOfRegisters);
        short crc = WordFunctions.CRC16.calculate(message, 0, 6);
        message[6] = WordFunctions.getLSByte(crc);
        message[7] = WordFunctions.getMSByte(crc);
        return 8;
    }

    /**
     * Creates a Modbus request to read holding registers.
     *
     * @param startAddress  the starting address of the holding registers to read
     * @param numOfRegisters  the number of holding registers to read
     * @return the size of the message
     */
    public short createReadHoldingRegistersRequest(short startAddress, short numOfRegisters) {
        // size is slaveID + function code + startAddress + numOfRegisters + CRC = 1+1+2+2+2=8
        message = new byte[8];
        message[0] = slaveID;
        message[1] = FunctionCodes.READ_HOLDING_REGISTERS_FUNCTIONCODE;
        message[2] = WordFunctions.getMSByte(startAddress);
        message[3] = WordFunctions.getLSByte(startAddress);
        message[4] = WordFunctions.getMSByte(numOfRegisters);
        message[5] = WordFunctions.getLSByte(numOfRegisters);

        // Calculate CRC for message
        short crc = WordFunctions.CRC16.calculate(message, 0, 6);
        message[6] = WordFunctions.getLSByte(crc);
        message[7] = WordFunctions.getMSByte(crc);

        return 8;
    }

    /**
     * In coils to send command for set we should send 0xFF00 as a value and for reset we should send 0x0000
     */
    public short createWriteSingleCoilRequest(short address, boolean value) {
        // size is slaveID + function code + address + value + CRC = 1+1+2+2+2=8
        message = new byte[8];
        message[0] = slaveID;
        message[1] = FunctionCodes.WRITE_SINGLE_COIL_FUNCTIONCODE;
        message[2] = WordFunctions.getMSByte(address);
        message[3] = WordFunctions.getLSByte(address);
        if (value) {
            message[4] = (byte) 0xFF;
            message[5] = 0x00;
        } else {
            message[4] = 0x00;
            message[5] = 0x00;
        }
        // Calculate CRC for message
        short crc = WordFunctions.CRC16.calculate(message, 0, 6);
        message[6] = WordFunctions.getLSByte(crc);
        message[7] = WordFunctions.getMSByte(crc);

        return 8;
    }

    public short createWriteCoilsRequest(short startAddress, boolean[] values, short quantity) {
        // size is slaveID + function code + start address + quantity + byte count + values in bytes + CRC = 1+1+2+2+1+more+2 = 7+more+2
        int message_size = 9;
        byte byteCount = (byte)(quantity / 8);
        if (quantity % 8 != 0) {
            byteCount++;
        }
        message_size += byteCount;
        message = new byte[message_size];
        message[0] = slaveID;
        message[1] = FunctionCodes.WRITE_MULTIPLE_COILS_FUNCTIONCODE;
        message[2] = WordFunctions.getMSByte(startAddress);
        message[3] = WordFunctions.getLSByte(startAddress);
        message[4] = WordFunctions.getMSByte(quantity);
        message[5] = WordFunctions.getLSByte(quantity);
        message[6] = byteCount;
        int currentPlaceInMessage = 7;
        byte[] myBytes = new byte[byteCount];
        for (int i = 0; i < quantity; i++) {
            // Calculate the index of the coil status byte that contains the current coil
            int byteIndex = i / 8;

            // Calculate the bit position of the current coil within the current coil status byte
            int bitPosition = i % 8;

            // Set the bit in the coil status byte if the current coil is true
            if (values[i]) {
                myBytes[byteIndex] |= (1 << bitPosition);
            }
        }
        for (int j = 0; j < byteCount; j++) {
            message[currentPlaceInMessage + j] = myBytes[j];
        }
        short crc = WordFunctions.CRC16.calculate(message, 0, message_size-2);
        message[message_size-2] = WordFunctions.getLSByte(crc);
        message[message_size-1] = WordFunctions.getMSByte(crc);
        return (short) message_size;
    }


    /**
     * Creates a Modbus request to write multiple registers.
     *
     * @param startAddress  the starting address of the registers to write
     * @param values  the values to write to the registers
     * @return the size of the message
     */
    public short createWriteRegistersRequest(short startAddress, short[] values , short numOfRegisters) {
        // size is slaveID + function code + startAddress + numOfRegisters + numBytes + values + CRC = 1+1+2+2+2+(2*numOfRegisters)+2
        int numBytes = 2 * numOfRegisters;
        int message_size= 9 + numBytes;
        message = new byte[message_size];
        message[0] = slaveID;
        message[1] = FunctionCodes.WRITE_MULTIPLE_REGISTERS_FUNCTIONCODE;
        message[2] = WordFunctions.getMSByte(startAddress);
        message[3] = WordFunctions.getLSByte(startAddress);
        message[4] = WordFunctions.getMSByte(numOfRegisters);
        message[5] = WordFunctions.getLSByte(numOfRegisters);
        message[6] = (byte) numBytes;
        for (int i = 0; i < numOfRegisters; i++) {
            message[7 + i * 2] = WordFunctions.getMSByte(values[i]);
            message[8 + i * 2] = WordFunctions.getLSByte(values[i]);
        }

        // Calculate CRC for message
        short crc = WordFunctions.CRC16.calculate(message, 0, 7 + numBytes);
        message[7 + numBytes] = WordFunctions.getLSByte(crc);
        message[8 + numBytes] = WordFunctions.getMSByte(crc);

        return (short) (message_size);
    }

    /**
     * Gets the Modbus message.
     *
     * @return the Modbus message
     */
    public byte[] getMessage() {
        return this.message;
    }
}

