#include "C:\Users\AEA\AppData\Local\Arduino15\libraries\MiniModbus\examples\Configs.h"
#include "MiniModbus.h"

#define MY_ID 0x01
#define NUM_OF_COILS 10
#define NUM_OF_DISCRETES 15


boolean coils[NUM_OF_COILS] {};
boolean discreteInputs[NUM_OF_DISCRETES] {};


Slave miniModbusSlave{MY_ID,Serial,PARSER_DIGITALS_BUFFER_SIZE,PARSER_REGISTERS_BUFFER_SIZE};

// ModbusRequestResponseParser parser(MY_ID
// Based on Uno
void setup()
{
    Serial.begin(19200);
    // Serial2.begin(19200);
    pinMode(13, OUTPUT);

    miniModbusSlave.setCoilsRefrence(coils,NUM_OF_COILS);
    miniModbusSlave.setDiscreteInputsRefrence(discreteInputs, NUM_OF_DISCRETES);
    discreteInputs[0]=0;
    discreteInputs[1]=0;
    discreteInputs[2]=1;
    discreteInputs[3]=1;
    discreteInputs[4]=0;
    discreteInputs[5]=1;
    discreteInputs[6]=0;
    discreteInputs[7]=1;
    discreteInputs[8]=1;
    discreteInputs[9]=1;
    discreteInputs[10]=0;
    discreteInputs[11]=0;
    discreteInputs[12]=1;
    discreteInputs[13]=0;
    discreteInputs[14]=1;

}

void loop()
{
    miniModbusSlave.execute();
    // Serial.print("Coil0:");
    // Serial.println(coils[0]);
    // Serial.print("Coil1:");
    // Serial.println(coils[1]);
    
    digitalWrite(13,coils[1]);
    
   
}
