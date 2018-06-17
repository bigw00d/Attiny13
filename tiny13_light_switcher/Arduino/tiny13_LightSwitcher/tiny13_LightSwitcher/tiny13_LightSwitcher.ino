#include <avr/io.h>
#include <avr/interrupt.h>

#define INTERRUPTPIN PCINT1 //this is PB1(tiny13a:MISO) per the schematic
#define PCINT_VECTOR PCINT0_vect  //this step is not necessary
#define PORTPIN PB1 //Page 64
#define DATADIRECTIONPIN DDB1 //Page 64 of data sheet
#define READPIN PINB1 //page 64

#define LED25_PIN 2
#define LED34_PIN 1
#define LED16_PIN 0
#define PIN 3
#define ON_TIME 1000
#define OFF_TIME 1000
#define WAIT_TIME 300
#define ON_DTIME (ON_TIME) // for CLOCK 1.2MHz
#define WAIT_DTIME (WAIT_TIME) // for CLOCK 1.2MHz

static volatile byte LEDState; //variable used within ISR must be declared Volatile.

void setup( ) {
  pinMode(PIN, OUTPUT);
  pinMode(LED16_PIN, OUTPUT);
  pinMode(LED25_PIN, OUTPUT);
  pinMode(LED34_PIN, OUTPUT);

  digitalWrite(LED16_PIN, LOW);
  digitalWrite(LED25_PIN, HIGH);
  digitalWrite(LED34_PIN, HIGH);

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
     digitalWrite(LED16_PIN, LOW);
     digitalWrite(LED25_PIN, HIGH);
     LEDState = 0;
     delay(ON_TIME);
   }
   else {
     digitalWrite(PIN, LOW);
     digitalWrite(LED16_PIN, HIGH);
     digitalWrite(LED25_PIN, LOW);
     LEDState = 1;
     delay(OFF_TIME);
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
