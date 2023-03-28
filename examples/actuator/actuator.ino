#include "MiniModbus.h"

#define MY_ID 0x02
#define NUM_OF_HOLDING_REGS 2


word holdingRegisters[NUM_OF_HOLDING_REGS] {};


Slave miniModbusSlave{MY_ID,Serial};

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
  digitalWrite(13,!digitalRead(13));
}
// Based on Uno
void setup()
{
    Serial.begin(19200);
    // Serial2.begin(19200);
    pinMode(13, OUTPUT);
    miniModbusSlave.setHoldingRegistersRefrence(holdingRegisters,NUM_OF_HOLDING_REGS);
    
    setupTimer1Interrupt(2000);
}

void loop()
{
    miniModbusSlave.execute();
    // Serial.print("Coil0:");
    // Serial.println(coils[0]);
    // Serial.print("Coil1:");
    // Serial.println(coils[1]);
    
    setupTimer1Interrupt(holdingRegisters[1]);
    
   
}


