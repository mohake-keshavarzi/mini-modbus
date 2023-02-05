#include "MiniModbus.h"


#define MY_ID 0x01
#define NUM_OF_COILS 2
#define NUM_OF_HOLDING_REGS 0
#define NUM_OF_INPUT_REGS 0

byte message[256];

boolean coils[NUM_OF_COILS]{};
word holdingRegs[NUM_OF_HOLDING_REGS],inputRegs[NUM_OF_INPUT_REGS];

ModbusResponseCreator myResponse(coils,NUM_OF_COILS
  ,holdingRegs,NUM_OF_HOLDING_REGS,inputRegs,NUM_OF_INPUT_REGS);

//ModbusRequestResponseParser parser(MY_ID
void setup() {
  Serial2.begin(19200);
  Serial.begin(19200);
  pinMode(23,OUTPUT);
}

void loop() {
  int messageSize= Serial2.available();
  int responseSize=0;
  if(messageSize>0){
    Serial.println("New message");
    Serial2.readBytes(message,messageSize);
    
    ModbusRequestResponseParser parser((uint8_t)MY_ID,message);
    if(parser.getSlaveID() == MY_ID){
      switch(parser.getFunctionCode()){
        case WRITE_SINGLE_COIL_FUNCTIONCODE:
            Serial.print("Requeste address is:");
            Serial.println(parser.getAddress());
          try{
            if(myResponse.isDataAddressWrong(parser.getAddress(),0,COILS)){
                Serial.println("Address is wrong");
                
                throw INVALID_DATA_ADDRESS_EXCEPTION_CODE;
                
              }
            responseSize=myResponse.createWriteSingleCoilResponse(parser.getAddress(),parser.getWriteSingleCoilValue());
          }catch(int exceptionCode){
            responseSize=myResponse.createExceptionResponse(WRITE_SINGLE_COIL_FUNCTIONCODE,exceptionCode);
          }
        break;
        case READ_INPUT_REGISTERS_FUNCTIONCODE:
          Serial.println("READ INPUT REGISTERS REQUEST");
          try
          {
            if(myResponse.isDataAddressWrong(parser.getAddress(),parser.getNumberOfRegsOrCoils(),INPUT_REGISTERS)) throw INVALID_DATA_ADDRESS_EXCEPTION_CODE;
            responseSize= myResponse.createReadInputRegistersResponse(parser.getAddress(),parser.getNumberOfRegsOrCoils());
          }
          catch(int exceptionCode)
          {
            responseSize=myResponse.createExceptionResponse(READ_INPUT_REGISTERS_FUNCTIONCODE,exceptionCode);
          }
          
          for(int i{0};i<responseSize;i++){
            Serial.println((int) myResponse.getMessage()[i],HEX);
          }
        break;
      }
      Serial.print("coil[0] value is");
      Serial.println(coils[0]);
//      
      Serial.print("TRY TO WRITE");
      Serial.println(responseSize);
      Serial2.write(myResponse.getMessage(),responseSize);
    }

  }
    Serial2.flush();
  digitalWrite(23,coils[0]);
  messageSize=0;
  delay(10);
}
