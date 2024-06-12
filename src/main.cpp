
/*
////        including liberaries
*/

#include "global.h"
#include "motor.h"
#include "servo.h"
#include "serial.h"


/*
////        Defining constants
*/

#define NUM_OF_COMMANDS 8
#define LEFT_SENSOR PC1
#define LEFT_START 120
#define RIGHT_SENSOR PC3
#define CENTER 90
#define REACH_DIFF 30


/*
////        Initializing variables
*/

Motor weapon(1);
Motor left(2);
Motor right(3);
Motor motors[3] = {left, right, weapon};
Servo servo(9);
bool manuallyOn = false;
bool reachIncrement;
uint8_t reach;


/*
////        List of rx commands
*/

uint8_t rxCommands[NUM_OF_COMMANDS][3] = {
//  {left    ,  right   , weapon  },          // Title          index     alphabet character
    {FORWARD ,  FORWARD , 0       },          // Forward        0         @
    {RELEASE ,  FORWARD , 0       },          // Left:          1         A
    {FORWARD ,  RELEASE , 0       },          // Right:         2         B
    {BACKWARD,  BACKWARD, 0       },          // Back:          3         C
    {RELEASE ,  RELEASE , 0       },          // Stop wheels:   4         D
    {0       ,  0       , FORWARD },          // Weapon on:     5         E
    {0       ,  0       , RELEASE },          // Weapon off:    6         F
    {RELEASE ,  RELEASE , RELEASE },          // STOP ALL:      7         G
};


/*
////        Prototypes
*/
void putOff(int direction);


/*
////        Main
*/

int main() {

  /*
    Start serial monitor
  */
  Serial::begin();

  /*
    Interrupts
  */

  // Enable global interrupts
  sei();

  // Enabling rx interrupt
  UCSR0B |= _BV(RXCIE0);


  /*
    sleep
  */

  // PRR = 0b11101101;       // turning off most of power consuming stuff
  // ADMUX = _BV(ADLAR) | _BV(REFS0); // setting the Adc data register to be left adjusted by the ADLAR pin, Setting also the voltage reference to internal

  // // Enabling sleep mode and setting the sleep mode to idle
  // SMCR |= _BV(SE);

  // // powering down the adc analog comprator
  // ACSR |= _BV(ACD);

  // // sleep instruction
  // sleep_cpu();

  /*
    Main loop
  */

  while (true)
  {
    []()
    {
      // For every sensor of the sensors
      for (char sensor = LEFT_SENSOR, start = LEFT_START; sensor <= RIGHT_SENSOR; sensor++, start -= 60)
      {

        // return if no fire detected from sensor digital input
        PORTC |= _BV(sensor); // set a pullover on pin 0 to read it by pinb
        // insert a nop
        _NOP();

        // if fire detected from that senosr
        if (bitIsClear(PINC, sensor))
        {

          // put off that fire then return
          putOff(start);
          return;
        }
      }

      // If no fire is detected and also the weapon not turned on manually
      if (!manuallyOn)
      {

        // make sure the weapon is not running
        weapon.run(RELEASE);

        // make the weapon look forward
        servo.write(CENTER);

        // Return reach to zero
        reach = 0;
      }
    }();
  }
  

}

ISR(USART_RX_vect) {
  // Get the operation code {index} by getting only the first five bits
  uint8_t index = Serial::read() & ~0b11100000;

  if (index > NUM_OF_COMMANDS) {
    return;
  }
  Serial::print("Recieved command: ");
  Serial::print(index);
  Serial::print('\n');

  // For every motor on the shield, execute the appropriate command
  for (char i = 0; i < 3; i++) {
    motors[i].run(rxCommands[index][i]);
  }

  // For the weapon, make sure to update values of manuallyOn
  switch (rxCommands[index][2]) {
      case 0:
        return;
      case FORWARD:
        manuallyOn = true;
        break;
      default:
        manuallyOn = false;
        break;
    }
}

void putOff(int direction) {
  
  // Turn on the fan
  weapon.run(FORWARD);

  switch (reachIncrement) {
    case true:
      if (reach == 60) {
        reachIncrement = false;
        return;
      }
      reach += REACH_DIFF;
      break;
    default:
      if (reach == 0) {
        reachIncrement = true;
        return;
      }
      reach -= REACH_DIFF;
      break; 
  }

  // Turn the servo to direction + reach
  servo.write(direction + reach);

}
uint8_t n = 0;

SIGNAL(TIMER1_COMPA_vect) {

   if( n < 0 )
    TCNT1 = 0; // channel set to -1 indicated that refresh interval completed so reset the timer
  else{
      PORTD &= ~_BV(servo.pin); // pulse this channel low
  }

  n++;    // increment to the next channel
  if( n < 1) {
    OCR1A = TCNT1 + servo.ticks;
    PORTD |= _BV(servo.pin); // it's an active channel so pulse it high
  }
  else {
    if( TCNT1 + 4 < usToTicks(REFRESH_INTERVAL) )  // allow a few ticks to ensure the next OCR1A not missed
      OCR1A = (unsigned int)usToTicks(REFRESH_INTERVAL);
    else
      OCR1A = TCNT1 + 4;  // at least REFRESH_INTERVAL has elapsed
    n -1; // this will get incremented at the end of the refresh period to start again at the first channel
  }

  // TCNT1 = 0;

  // PORTD &= ~_BV(servo.pin);

  // OCR1A = TCNT1 + servo.ticks;

  // PORTD |= _BV(servo.pin);

  // if (TCNT1 + 4 < usToTicks(REFRESH_INTERVAL)) // allow a few ticks to ensure the next OCR1A not missed
  //   OCR1A = (unsigned int)usToTicks(REFRESH_INTERVAL);
  // else
  //   OCR1A = TCNT1 + 4; // at least REFRESH_INTERVAL has elapsed
}