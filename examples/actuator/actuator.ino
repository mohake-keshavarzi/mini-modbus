#include "C:\Users\AEA\AppData\Local\Arduino15\libraries\MiniModbus\examples\Configs.h"
#include "MiniModbus.h"

#define MY_ID 0x02
#define NUM_OF_HOLDING_REGS 10
#define NUM_OF_INPUT_REGS 10


word holdingRegisters[NUM_OF_HOLDING_REGS] {};
word inputRegisters[NUM_OF_INPUT_REGS] {};


Slave miniModbusSlave{MY_ID,Serial,PARSER_DIGITALS_BUFFER_SIZE,PARSER_REGISTERS_BUFFER_SIZE};

void setupTimer1Interrupt(unsigned long delayTime) {
  cli(); // Disable global interrupts

  TCCR1A = 0; // Clear Timer1 control registers
  TCCR1B = 0;

  // Set Timer1 to CTC (Clear Timer on Compare Match) mode
  TCCR1B |= (1 << WGM12);

  // Set Timer1 prescaler to 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);

  // Calculate value for Timer1 Compare Match register based on delay time
  unsigned long compareMatchValue = (delayTime * 1000) / (1000000 / (F_CPU / 1024));
  OCR1A = compareMatchValue;

  // Enable Timer1 Compare Match interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei(); // Enable global interrupts
}

// Timer1 interrupt service routine (ISR)
ISR(TIMER1_COMPA_vect) {
  // digitalWrite(13,!digitalRead(13));
  for(int i{0};i<NUM_OF_HOLDING_REGS;i++) holdingRegisters[i]=i*random(100,150);
  for(int i{0};i<NUM_OF_INPUT_REGS;i++) inputRegisters[i]=i;  
  if(digitalRead(13)) digitalWrite(13,false);

}
// Based on Uno
void setup()
{
    Serial.begin(115200);
    // Serial2.begin(19200);
    pinMode(13, OUTPUT);
    miniModbusSlave.setHoldingRegistersRefrence(holdingRegisters,NUM_OF_HOLDING_REGS);
    miniModbusSlave.setInputRegistersRefrence(inputRegisters, NUM_OF_INPUT_REGS);
    setupTimer1Interrupt(500);
}

void loop()
{
    miniModbusSlave.execute();
    
    // Serial.print("Coil0:");
    // Serial.println(coils[0]);
    // Serial.print("Coil1:");
    // Serial.println(coils[1]);
    // for(int i{0};i<NUM_OF_HOLDING_REGS;i++) {
    //   Serial.print(holdingRegisters[i]);
    //   Serial.print("    ");
    // }      
    // Serial.println();

    
}

