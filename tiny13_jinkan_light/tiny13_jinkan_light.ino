#include <avr/io.h>
#include <avr/interrupt.h>

#define INTERRUPTPIN PCINT1 //this is PB1(tiny13a:MISO) per the schematic
#define PCINT_VECTOR PCINT0_vect  //this step is not necessary
#define PORTPIN PB1 //Page 64
#define DATADIRECTIONPIN DDB1 //Page 64 of data sheet
#define READPIN PINB1 //page 64

#define PIN 3
#define ON_TIME 10000
#define WAIT_TIME 300
#define ON_DTIME (ON_TIME) // for CLOCK 1.2MHz
#define WAIT_DTIME (WAIT_TIME) // for CLOCK 1.2MHz

static volatile byte LEDState; //variable used within ISR must be declared Volatile.

void setup( ) {
  pinMode(PIN, OUTPUT);
  
  cli();//disable interrupts during setup
  PCMSK |= (1 << INTERRUPTPIN); //sbi(PCMSK,INTERRUPTPIN) also works but I think this is more clear // tell pin change mask to listen to pin2 /pb3 //SBI
  GIMSK |= (1 << PCIE);   // enable PCINT interrupt in the general interrupt mask //SBI
  
  DDRB &= ~(1 << DATADIRECTIONPIN); //cbi(DDRB, DATADIRECTIONPIN);//  set up as input  - pin2 clear bit  - set to zero
  PORTB |= (1<< PORTPIN); //cbi(PORTB, PORTPIN);// disable pull-up. hook up pulldown resistor. - set to zero
  LEDState = 0;
  sei(); //last line of setup - enable interrupts after setup

}
 
void loop( ) {
   if (LEDState == 1) {
     digitalWrite(PIN, HIGH);
     LEDState = 0;
     delay(ON_DTIME);
   }
   else {
     digitalWrite(PIN, LOW);
   }
   delay(WAIT_DTIME);
}

//this is the interrupt handler
ISR(PCINT_VECTOR)
{
  if (LEDState == 0)
  {
    LEDState = 1; 
  }
}
