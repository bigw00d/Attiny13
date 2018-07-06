#include <avr/io.h>
#include <avr/interrupt.h>

#define USE_3LED

//#define INTERRUPTPIN PCINT1 //this is PB1(tiny13a:MISO) per the schematic
#define INTERRUPTPIN PCINT3 //this is PB3(Pin No.2) per the schematic
#define PCINT_VECTOR PCINT0_vect  //this step is not necessary
//#define PORTPIN PB1 //Page 64
#define PORTPIN PB3 //Page 64
//#define DATADIRECTIONPIN DDB1 //Page 64 of data sheet
#define DATADIRECTIONPIN DDB3 //Page 64 of data sheet
//#define READPIN PINB1 //page 64
#define READPIN PORTB3 //page 64

#define LED25_PIN 2
#define LED34_PIN 1
#define LED16_PIN 0
#define PIN 3
#define ON_TIME 3000 // for CLOCK 1.2MHz
#define OFF_TIME 3000 // for CLOCK 1.2MHz
#define WAIT_TIME 10 // for CLOCK 1.2MHz
#define BLINK_TIME 50 // for CLOCK 1.2MHz

#define MAX_LED_STATE 6
#define ACTIVE 0x00
#define INACTIVE 0xFF
static byte SwitchCtrl; //variable used within ISR must be declared Volatile.
static byte LEDState2; //variable used within ISR must be declared Volatile.
static byte StateUpdate; //variable used within ISR must be declared Volatile.
static volatile byte SwitchPushed; //variable used within ISR must be declared Volatile.
static byte SwitchCheckCount; //variable used within ISR must be declared Volatile.
#define SWITCH_KAKUTEI_COUNT 10 // this number x WAIT_TIME = state update time
static byte BlinkCount; //variable used within ISR must be declared Volatile.
#define BLINK_CHANGE_COUNT 10 // this number x WAIT_TIME = state update time


void setup( ) {
  digitalWrite(LED16_PIN, LOW);
  digitalWrite(LED25_PIN, LOW);
  digitalWrite(LED34_PIN, LOW);

  pinMode(PIN, OUTPUT);
  pinMode(LED16_PIN, OUTPUT);
  pinMode(LED25_PIN, OUTPUT);
  pinMode(LED34_PIN, OUTPUT);

  LEDState2 = 0;

  // settings for interrupts >
  #if 1
  cli();//disable interrupts during setup
  PCMSK |= (1 << INTERRUPTPIN); //sbi(PCMSK,INTERRUPTPIN) also works but I think this is more clear // tell pin change mask to listen to pin2 /pb3 //SBI
  GIMSK |= (1 << PCIE);   // enable PCINT interrupt in the general interrupt mask //SBI
  
  DDRB &= ~(1 << DATADIRECTIONPIN); //cbi(DDRB, DATADIRECTIONPIN);//  set up as input  - pin2 clear bit  - set to zero
  PORTB |= (1<< PORTPIN); //cbi(PORTB, PORTPIN);// disable pull-up. hook up pulldown resistor. - set to zero
  SwitchCtrl = ACTIVE; //Switch Control is active
  SwitchPushed = 0;
  SwitchCheckCount = 0;
  StateUpdate = 0;
  BlinkCount = 0;
  sei(); //last line of setup - enable interrupts after setup
  #else
  SwitchCtrl = INACTIVE; //Switch Control is inactive
  StateUpdate = 1;
  #endif
  // < settings for interrupts

}
 
void loop( ) {

  if(StateUpdate) {
    StateUpdate = 0;
    switch(LEDState2) {
      case 0:
        digitalWrite(LED16_PIN, LOW);
        digitalWrite(LED25_PIN, LOW);
        digitalWrite(LED34_PIN, LOW);
        break;
      case 1:
        digitalWrite(LED16_PIN, LOW);
        digitalWrite(LED25_PIN, HIGH);
        digitalWrite(LED34_PIN, LOW);
        break;
      case 2:
        digitalWrite(LED16_PIN, LOW);
        digitalWrite(LED25_PIN, LOW);
        digitalWrite(LED34_PIN, HIGH);
        break;
      case 3:
        digitalWrite(LED16_PIN, HIGH);
        digitalWrite(LED25_PIN, LOW);
        digitalWrite(LED34_PIN, HIGH);
        break;
      case 4:
        digitalWrite(LED16_PIN, HIGH);
        digitalWrite(LED25_PIN, HIGH);
        digitalWrite(LED34_PIN, HIGH);
        BlinkCount = 0;
        break;
      case 5:
        digitalWrite(LED16_PIN, HIGH);
        digitalWrite(LED25_PIN, LOW);
        digitalWrite(LED34_PIN, LOW);
        StateUpdate = 1;
        BlinkCount++;
        if(BlinkCount >= BLINK_CHANGE_COUNT) {
          BlinkCount = 0;
          LEDState2 = 6;
        }
        delay(BLINK_TIME);
        break;
      case 6:
        digitalWrite(LED16_PIN, HIGH);
        digitalWrite(LED25_PIN, LOW);
        digitalWrite(LED34_PIN, HIGH);
        StateUpdate = 1;
        BlinkCount++;
        if(BlinkCount >= BLINK_CHANGE_COUNT) {
          BlinkCount = 0;
          LEDState2 = 7;
        }
        delay(BLINK_TIME);
        break;
      case 7:
        digitalWrite(LED16_PIN, LOW);
        digitalWrite(LED25_PIN, HIGH);
        digitalWrite(LED34_PIN, LOW);
        StateUpdate = 1;
        BlinkCount++;
        if(BlinkCount >= BLINK_CHANGE_COUNT) {
          BlinkCount = 0;
          LEDState2 = 5;
        }
        delay(BLINK_TIME);
        break;
      default:
        break;
    }
  }

  if(SwitchCtrl == INACTIVE) {
    LEDState2++;
    LEDState2 %= MAX_LED_STATE;
    delay(ON_TIME);
  }
  else {
    if(SwitchPushed == 1) {
      SwitchCheckCount++;
      if(SwitchCheckCount >= SWITCH_KAKUTEI_COUNT) {
        SwitchPushed = 0;
        SwitchCheckCount = 0;
        LEDState2++;
        if(LEDState2 >= MAX_LED_STATE) {
          LEDState2 = 0;
        }
        StateUpdate = 1;
      }
    }
  }

  delay(WAIT_TIME);
}

#if 1
//this is the interrupt handler
ISR(PCINT_VECTOR)
{
  if(bit_is_set(PINB, READPIN)){
    ; //No update when pin is low Level
  }  
  else {
    SwitchPushed = 1; //Update when pin is low Level
  }
}
#endif
