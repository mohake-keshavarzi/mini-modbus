package ir.m.h.keshavarzi.minimodbus;


import android.util.Log;

import java.util.ArrayList;

import ir.m.h.keshavarzi.smartlineagv.usb.USBCommunication;

public class MiniModbusMaster {
    private static final String TAG = MiniModbusMaster.class.getSimpleName();

    private final USBCommunication usbCommunication;
    private final static int BUFFER_SIZE= 256;
//    private byte[] responseBuffer;
    private String serialDeviceName;
    private int timeout=300;
    private int numberOfTries =2;
    private int writeDelayTime=300;
    private ReadThread readThread;

    private final ModbusRequestResponseParser parser;

    public MiniModbusMaster(int parserDigitalsBufferSize,int parserRegistersBufferSize){
        usbCommunication=USBCommunication.getInstance();

        parser=new ModbusRequestResponseParser(null,parserDigitalsBufferSize,parserRegistersBufferSize);

    }

    public boolean isSerialConfigured(){
        return usbCommunication.isSerialDeviceCreated();
    }

    public void openPort(String portName){
        if(!usbCommunication.isSerialDeviceCreated()){
            throw new IllegalStateException("No Serial device exists to work with");
        }
        if(usbCommunication.isPortOpened()) return;
        usbCommunication.openSerialSync(portName);
        readThread = new ReadThread();
        readThread.start();
    }
    public void closePort(){
        if(readThread !=null)
            readThread.finish();
        if(usbCommunication.isPortOpened())
            usbCommunication.closePortSync();
    }

    public void setReadingPause(boolean pause){
        if(readThread!=null){
            readThread.setPause(pause);
        }
    }

    public void writeSingleCoil(byte slaveID,short address,boolean value)
            throws MiniModbusCommunicationFailureException {
        ModbusRequestCreator slave= new ModbusRequestCreator(slaveID);
        int tries = numberOfTries;
        int currentTry=0;
        boolean failed=true;
        if(!usbCommunication.isPortOpened())
            throw new IllegalStateException("No open port exists to communicate");
        while (tries >0 && failed) {
            currentTry++;
            tries--;
            readThread.flushBuffer();
            int requestSize = slave.createWriteSingleCoilRequest(address, value);
            if (!usbCommunication.isPortOpened())
                throw new IllegalStateException("No open port to send command to");
            usbCommunication.syncSendBytes(slave.getMessage(), timeout);
            try {
                Thread.sleep(writeDelayTime);
            }catch (InterruptedException e){
                e.printStackTrace();
            }
            if(readThread.isEmpty()) continue;
            parser.setMessage(readThread.getResponse());
            try {
                if(!parser.isValidCRC(parser.getMessage_size())){
                    Log.d(TAG, "[TRY:" + currentTry + "] Wrong CRC");
                    continue;
                }
                if (messageIsInvalid(parser, FunctionCodes.MASTER_ID, FunctionCodes.WRITE_SINGLE_COIL_FUNCTIONCODE)) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Received message is not referred to master or has an unmatched FunctionCode");
                    continue;
                }
                if (parser.isItException())
                    throw new ModbusException("An exception related to Modbus protocol happened: "
                            +  String.format("%02X ", parser.getExceptionCode())
                            , parser.getExceptionCode());

                if(parser.getAddress() != address){
                    Log.d(TAG, "[TRY:" + currentTry + "]Response address doesn't match with requested address");
                    continue;
                }
                if (parser.getWriteSingleCoilValue() == FunctionCodes.COIL_VALUE_ERROR) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Unacceptable value for coil value is shown in the response");
                    continue;
                }
                if (value !=
                        (parser.getWriteSingleCoilValue() == FunctionCodes.COIL_VALUE_SET)) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Response shows a value unmatched to requested value");
                    continue;
                }
            }catch (ArrayIndexOutOfBoundsException e){
                e.printStackTrace();
                continue;
            }
            failed = false;

        }
        if(failed) throw new MiniModbusCommunicationFailureException(currentTry);
        Log.d(TAG,"Successful communication for [Write Single Coil]");
    }

    public void writeSingleRegister(byte slaveID,short address,short value)
            throws MiniModbusCommunicationFailureException {
        ModbusRequestCreator slave= new ModbusRequestCreator(slaveID);
        int tries = numberOfTries;
        int currentTry=0;
        boolean failed=true;
        if(!usbCommunication.isPortOpened())
            throw new IllegalStateException("No open port exists to communicate");
        while (tries >0 && failed) {
            currentTry++;
            tries--;
            readThread.flushBuffer();
            int requestSize = slave.createWriteSingleRegisterRequest(address, value);
            if (!usbCommunication.isPortOpened())
                throw new IllegalStateException("No open port to send command to");
            usbCommunication.syncSendBytes(slave.getMessage(), timeout);
            try {
                Thread.sleep(writeDelayTime);
            }catch (InterruptedException e){
                e.printStackTrace();
            }
            if(readThread.isEmpty()) continue;
            parser.setMessage(readThread.getResponse());
            try {
                if(!parser.isValidCRC(parser.getMessage_size())){
                    Log.d(TAG, "[TRY:" + currentTry + "] Wrong CRC");
                    continue;
                }
                if (messageIsInvalid(parser, FunctionCodes.MASTER_ID, FunctionCodes.WRITE_SINGLE_REGISTER_FUNCTIONCODE)) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Received message is not referred to master or has an unmatched FunctionCode");
                    continue;
                }
                if (parser.isItException())
                    throw new ModbusException("An exception related to Modbus protocol happened: "
                            +  String.format("%02X ", parser.getExceptionCode())
                            , parser.getExceptionCode());

                if(parser.getAddress() != address){
                    Log.d(TAG, "[TRY:" + currentTry + "]Response address doesn't match with requested address");
                    continue;
                }
                if (parser.getWriteSingleRegisterValue() != value) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Unacceptable value for coil value is shown in the response");
                    continue;
                }

            }catch (ArrayIndexOutOfBoundsException e){
                e.printStackTrace();
                continue;
            }
            failed = false;

        }
        if(failed) throw new MiniModbusCommunicationFailureException(currentTry);
        Log.d(TAG,"Successful communication for [Write Single Register]");
    }

    public short[] readInputRegisters(byte slaveID,short startAddress,short quantity)
            throws MiniModbusCommunicationFailureException {
        ModbusRequestCreator slave= new ModbusRequestCreator(slaveID);
        short[] result = new short[quantity];
        int tries = numberOfTries;
        int currentTry=0;
        boolean failed=true;
        if(!usbCommunication.isPortOpened())
            throw new IllegalStateException("No open port exists to communicate");
        while (tries >0 && failed) {
            currentTry++;
            tries--;
            readThread.flushBuffer();
            int requestSize = slave.createReadInputRegistersRequest(startAddress,quantity);
            if (!usbCommunication.isPortOpened())
                throw new IllegalStateException("No open port to send command to");
            usbCommunication.syncSendBytes(slave.getMessage(), timeout);
            try {
                Thread.sleep(writeDelayTime);
            }catch (InterruptedException e){
                e.printStackTrace();
            }
            if(readThread.isEmpty()) continue;
            parser.setMessage(readThread.getResponse());
            try {
                if(!parser.isValidCRC(parser.getMessage_size())){
                    Log.d(TAG, "[TRY:" + currentTry + "] Wrong CRC");
                    continue;
                }
                if (messageIsInvalid(parser, FunctionCodes.MASTER_ID, FunctionCodes.READ_INPUT_REGISTERS_FUNCTIONCODE)) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Received message is not referred to master or has an unmatched FunctionCode");
                    continue;
                }
                if (parser.isItException())
                    throw new ModbusException("An exception related to Modbus protocol happened: "
                            +  String.format("%02X ", parser.getExceptionCode())
                            , parser.getExceptionCode());

                if (parser.getByteCountInResponse() != 2*quantity) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Unacceptable number of bytes");
                    continue;
                }
                result = parser.getRegisterValuesArray();
            }catch (ArrayIndexOutOfBoundsException e){
                e.printStackTrace();
                continue;
            }
            failed = false;
        }
        if(failed) throw new MiniModbusCommunicationFailureException(currentTry);
        Log.d(TAG,"Successful communication for [Read multiple input registers]");
        return result;

    }

    public short[] readHoldingRegisters(byte slaveID, short startAddress, short quantity)
            throws MiniModbusCommunicationFailureException {
        ModbusRequestCreator slave = new ModbusRequestCreator(slaveID);
        short[] result = new short[quantity];
        int tries = numberOfTries;
        int currentTry = 0;
        boolean failed = true;
        if (!usbCommunication.isPortOpened())
            throw new IllegalStateException("No open port exists to communicate");
        while (tries > 0 && failed) {
            currentTry++;
            tries--;
            readThread.flushBuffer();
            int requestSize = slave.createReadHoldingRegistersRequest(startAddress, quantity);
            if (!usbCommunication.isPortOpened())
                throw new IllegalStateException("No open port to send command to");
            usbCommunication.syncSendBytes(slave.getMessage(), timeout);
            try {
                Thread.sleep(writeDelayTime);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            if (readThread.isEmpty())
                continue;
            parser.setMessage(readThread.getResponse());
            try {
                if (!parser.isValidCRC(parser.getMessage_size())) {
                    Log.d(TAG, "[TRY:" + currentTry + "] Wrong CRC");
                    continue;
                }
                if (messageIsInvalid(parser, FunctionCodes.MASTER_ID, FunctionCodes.READ_HOLDING_REGISTERS_FUNCTIONCODE)) {
                    Log.d(TAG, "[TRY:" + currentTry + "] Received message is not referred to master or has an unmatched FunctionCode");
                    continue;
                }
                if (parser.isItException())
                    throw new ModbusException("An exception related to Modbus protocol happened: "
                            +  String.format("%02X ", parser.getExceptionCode())
                            , parser.getExceptionCode());

                if (parser.getByteCountInResponse() != 2 * quantity) {
                    Log.d(TAG, "[TRY:" + currentTry + "] Unacceptable number of bytes");
                    continue;
                }
                result = parser.getRegisterValuesArray();
            } catch (ArrayIndexOutOfBoundsException e) {
                e.printStackTrace();
                continue;
            }
            failed = false;
        }
        if (failed)
            throw new MiniModbusCommunicationFailureException(currentTry);
        Log.d(TAG, "Successful communication for [Read multiple holding registers]");
        return result;
    }


    public boolean[] readCoils(byte slaveID, short startAddress, short quantity)
            throws MiniModbusCommunicationFailureException {
        ModbusRequestCreator slave = new ModbusRequestCreator(slaveID);
        boolean[] result = new boolean[quantity];
        int tries = numberOfTries;
        int currentTry = 0;
        boolean failed = true;
        if (!usbCommunication.isPortOpened())
            throw new IllegalStateException("No open port exists to communicate");
        while (tries > 0 && failed) {
            currentTry++;
            tries--;
            readThread.flushBuffer();
            int requestSize = slave.createReadCoilsRequest(startAddress, quantity);
            if (!usbCommunication.isPortOpened())
                throw new IllegalStateException("No open port to send command to");
            usbCommunication.syncSendBytes(slave.getMessage(), timeout);
            try {
                Thread.sleep(writeDelayTime);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            if (readThread.isEmpty())
                continue;
            parser.setMessage(readThread.getResponse());
            try {
                if (!parser.isValidCRC(parser.getMessage_size())) {
                    Log.d(TAG, "[TRY:" + currentTry + "] Wrong CRC");
                    continue;
                }
                if (messageIsInvalid(parser, FunctionCodes.MASTER_ID, FunctionCodes.READ_COILS_FUNCTIONCODE)) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Received message is not referred to master or has an unmatched FunctionCode");
                    continue;
                }
                if (parser.isItException())
                    throw new ModbusException("An exception related to Modbus protocol happened: "
                            +  String.format("%02X ", parser.getExceptionCode())
                            , parser.getExceptionCode());

                if (parser.getByteCountInResponse() != (quantity + 7) / 8) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Unacceptable number of bytes");
                    continue;
                }
                result = parser.getDigitalValuesArray();
            } catch (ArrayIndexOutOfBoundsException e) {
                e.printStackTrace();
                continue;
            }
            failed = false;
        }
        if (failed)
            throw new MiniModbusCommunicationFailureException(currentTry);
        Log.d(TAG, "Successful communication for [Read coils]");
        return result;
    }

    public boolean[] readDiscreteInputs(byte slaveID, short startAddress, short quantity)
            throws MiniModbusCommunicationFailureException {
        ModbusRequestCreator slave = new ModbusRequestCreator(slaveID);
        boolean[] result = new boolean[quantity];
        int tries = numberOfTries;
        int currentTry = 0;
        boolean failed = true;
        if (!usbCommunication.isPortOpened())
            throw new IllegalStateException("No open port exists to communicate");
        while (tries > 0 && failed) {
            currentTry++;
            tries--;
            readThread.flushBuffer();
            int requestSize = slave.createReadDiscreteInputsRequest(startAddress, quantity);
            if (!usbCommunication.isPortOpened())
                throw new IllegalStateException("No open port to send command to");
            usbCommunication.syncSendBytes(slave.getMessage(), timeout);
            try {
                Thread.sleep(writeDelayTime);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            if (readThread.isEmpty())
                continue;
            parser.setMessage(readThread.getResponse());
            try {
                if (!parser.isValidCRC(parser.getMessage_size())) {
                    Log.d(TAG, "[TRY:" + currentTry + "] Wrong CRC");
                    continue;
                }
                if (messageIsInvalid(parser, FunctionCodes.MASTER_ID, FunctionCodes.READ_DISCRETE_INPUTS_FUNCTIONCODE)) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Received message is not referred to master or has an unmatched FunctionCode");
                    continue;
                }
                if (parser.isItException())
                    throw new ModbusException("An exception related to Modbus protocol happened: "
                            +  String.format("%02X ", parser.getExceptionCode())
                            , parser.getExceptionCode());

                if (parser.getByteCountInResponse() != (quantity + 7) / 8) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Unacceptable number of bytes");
                    continue;
                }
                result = parser.getDigitalValuesArray();
            } catch (ArrayIndexOutOfBoundsException e) {
                e.printStackTrace();
                continue;
            }
            failed = false;
        }
        if (failed)
            throw new MiniModbusCommunicationFailureException(currentTry);
        Log.d(TAG, "Successful communication for [Read Discrete Inputs]");
        return result;
    }


    public void writeCoils(byte slaveID, short startAddress, boolean[] values, short quantity)
            throws MiniModbusCommunicationFailureException {
        ModbusRequestCreator slave = new ModbusRequestCreator(slaveID);
        boolean failed = true;
        int tries = numberOfTries;
        int currentTry = 0;
        if (!usbCommunication.isPortOpened())
            throw new IllegalStateException("No open port exists to communicate");
        while (tries > 0 && failed) {
            currentTry++;
            tries--;
            readThread.flushBuffer();
            int requestSize = slave.createWriteCoilsRequest(startAddress, values, quantity);
            if (!usbCommunication.isPortOpened())
                throw new IllegalStateException("No open port to send command to");
            usbCommunication.syncSendBytes(slave.getMessage(), timeout);
            try {
                Thread.sleep(writeDelayTime);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            if (readThread.isEmpty())
                continue;
            parser.setMessage(readThread.getResponse());
            try {
                if (!parser.isValidCRC(parser.getMessage_size())) {
                    Log.d(TAG, "[TRY:" + currentTry + "] Wrong CRC");
                    continue;
                }
                if (messageIsInvalid(parser, FunctionCodes.MASTER_ID, FunctionCodes.WRITE_MULTIPLE_COILS_FUNCTIONCODE)) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Received message is not referred to master or has an unmatched FunctionCode");
                    continue;
                }
                if (parser.isItException())
                    throw new ModbusException("An exception related to Modbus protocol happened: "
                            +  String.format("%02X ", parser.getExceptionCode())
                            , parser.getExceptionCode());
                if (parser.getAddress() != startAddress) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Received message has an incorrect start address");
                    continue;
                }
                if (parser.getNumberOfRegsOrDigitals() != quantity) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Received message has an incorrect quantity");
                    continue;
                }
                failed = false;
            } catch (ArrayIndexOutOfBoundsException e) {
                e.printStackTrace();
                continue;
            }
        }
        if (failed) throw new MiniModbusCommunicationFailureException(currentTry);
        Log.d(TAG, "Successful communication for [Write multiple coils]");
    }

    public void writeHoldingRegisters(byte slaveID, short startAddress, short[] values, short quantity) throws MiniModbusCommunicationFailureException {
        ModbusRequestCreator slave = new ModbusRequestCreator(slaveID);
        boolean failed = true;
        int tries = numberOfTries;
        int currentTry = 0;
        if(!usbCommunication.isPortOpened())
            throw new IllegalStateException("No open port exists to communicate");

        while (tries > 0 && failed) {
            currentTry++;
            tries--;
            readThread.flushBuffer();
            int requestSize = slave.createWriteRegistersRequest(startAddress, values, quantity);
            if (!usbCommunication.isPortOpened())
                throw new IllegalStateException("No open port to send command to");
            usbCommunication.syncSendBytes(slave.getMessage(), timeout);
            try {
                Thread.sleep(writeDelayTime);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            if(readThread.isEmpty()) continue;
            parser.setMessage(readThread.getResponse());

            try {
                if(!parser.isValidCRC(parser.getMessage_size())){
                    Log.d(TAG, "[TRY:" + currentTry + "] Wrong CRC");
                    continue;
                }
                if (messageIsInvalid(parser, FunctionCodes.MASTER_ID, FunctionCodes.WRITE_MULTIPLE_REGISTERS_FUNCTIONCODE)) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Received message is not referred to master or has an unmatched FunctionCode");
                    continue;
                }
                if (parser.isItException())
                    throw new ModbusException("An exception related to Modbus protocol happened: "
                            +  String.format("%02X ", parser.getExceptionCode())
                            , parser.getExceptionCode());

                if (parser.getAddress() != startAddress) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Received message has an unmatched start address");
                    continue;
                }
                if (parser.getNumberOfRegsOrDigitals() != quantity) {
                    Log.d(TAG, "[TRY:" + currentTry + "]Received message has an unmatched number of registers");
                    continue;
                }
                failed = false;
            } catch (ArrayIndexOutOfBoundsException e) {
                e.printStackTrace();
                continue;
            }
        }
        if(failed) throw new MiniModbusCommunicationFailureException(currentTry);
        Log.d(TAG,"Successful communication for [Write multiple holding registers]");
    }


    public void setTimeout(int timeout) {
        this.timeout = timeout;
    }

    private void setNumberOfTries(int num){
        this.numberOfTries =num;
    }


    private boolean messageIsInvalid(ModbusRequestResponseParser parser, byte myID, byte expectedFunctionCode) {
        if (myID != parser.getSlaveID()) return true;
        if (parser.isItException()) {
//            String s="";
//            s+= String.format("%02X ", (byte)(expectedFunctionCode + FunctionCodes.EXCEPTION_OFFSET));
//            Log.d(TAG,"Has Exception:"+ s);
            return parser.getFunctionCode() != (byte) (expectedFunctionCode + FunctionCodes.EXCEPTION_OFFSET);
        }
        return parser.getFunctionCode() != expectedFunctionCode;

    }

    public void setWriteDelayTime(int writeDelayTime) {
        this.writeDelayTime = writeDelayTime;
    }

    private class ReadThread extends Thread {
        ArrayList<Byte> responseBuffer= new ArrayList<>();
        private boolean finished=false;
        private boolean pause = false;
        @Override
        public void run() {
            while (!finished) {
                if(pause) continue;
                if(!usbCommunication.isPortOpened()) continue;
                if(!usbCommunication.getIsSyncMode()) continue;
                byte[] buffer = new byte[BUFFER_SIZE];
                int n = usbCommunication.syncReadBytes(buffer, timeout);
                if (n > 0) {
                    for (int i=0;i<n;i++) {
                        responseBuffer.add(buffer[i]);
                    }
//                    Log.d(TAG,new String(buffer,0, n, StandardCharsets.UTF_8));
                }

            }
        }

        public byte[] getResponse(){

            byte[] buffer = new byte[responseBuffer.size()];
            Log.d(TAG,"Buffer size:"+ responseBuffer.size());

            int i=0;
            for (Byte b: responseBuffer) {
                buffer[i]=b;
                i++;
            }
            Log.d(TAG,"received message:"+ bytesToHexFormatString(buffer,buffer.length));
            flushBuffer();
            return buffer;
        }

        public void flushBuffer(){
            responseBuffer.clear();
        }

        public boolean isEmpty(){
            return responseBuffer.isEmpty();
        }

        public void finish(){
            finished=true;
        }
        public void setPause(boolean pause){
            this.pause=pause;
        }

        private String bytesToHexFormatString(byte[] byteArray,int length){
            StringBuilder s= new StringBuilder();
            for (int i=0;i<length;i++) {
                s.append(String.format("%02X ", byteArray[i]));
            }
            return s.toString();
        }


    }
}
