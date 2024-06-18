
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

#define NUM_OF_COMMANDS 11
#define LEFT_SENSOR PC1
#define LEFT_START 120
#define RIGHT_SENSOR PC3
#define REACH_DIFF 15
#define LB 0
#define RB 2
#define WB 4


/*
////        Initializing variables
*/

Motor weapon(1);
Motor left(2);
Motor right(3);
Motor motors[3] = {left, right, weapon};
Servo servo(9);
bool manuallyOn = false;
bool reachIncrements[3] = {true, true, false};
uint8_t reaches[3] = {0, 0, 60};


/*
////        List of rx commands
*/

uint8_t rxCommands[NUM_OF_COMMANDS] = {
//  (left     << LB) | (right    << RB) | (weapon   << WB),          // Title         Index     Alphabet Character
    (FORWARD  << LB) | (FORWARD  << RB) | (IGNORE   << WB),          // Forward        0         @
    (FORWARD  << LB) | (RELEASE  << RB) | (IGNORE   << WB),          // Right:         1         A
    (RELEASE  << LB) | (FORWARD  << RB) | (IGNORE   << WB),          // Left:          2         B
    (BACKWARD << LB) | (BACKWARD << RB) | (IGNORE   << WB),          // Back:          3         C
    (RELEASE  << LB) | (RELEASE  << RB) | (IGNORE   << WB),          // Stop wheels:   4         D
    (IGNORE   << LB) | (IGNORE   << RB) | (FORWARD  << WB),          // Weapon on:     5         E
    (IGNORE   << LB) | (IGNORE   << RB) | (BACKWARD << WB),          // Weapon back:   6         F
    (IGNORE   << LB) | (IGNORE   << RB) | (RELEASE  << WB),          // Weapon off:    7         G
    (RELEASE  << LB) | (RELEASE  << RB) | (RELEASE  << WB),          // STOP ALL:      8         H
    (FORWARD  << LB) | (BACKWARD << RB) | (IGNORE   << WB),          // Super right    9         I
    (BACKWARD << LB) | (FORWARD  << RB) | (IGNORE   << WB),          // Super left     10        J
};

/*
////        Prototypes
*/
void putOff(int direction, uint8_t* reach, bool* reachIncrement);


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

          uint8_t index = sensor - LEFT_SENSOR;
          if (!servo.manual)
            // put off that fire then return
            putOff(start, &reaches[index], &reachIncrements[index]);
          return;
        }
      }

      // If no fire is detected and also the weapon not turned on manually
      if (!manuallyOn)
        // make sure the weapon is not running
        weapon.run(RELEASE);
      
      if (!servo.manual)
        // make the weapon look forward
        servo.write(CENTER);

        // Return reach to zero
      for (uint8_t i = 0; i < 2; i++) {
        reaches[i] = 0;
        reachIncrements[i] = true;
      }
      reaches[2] = 60;
      reachIncrements[2] = false;
      
    }();
    }
  

}

ISR(USART_RX_vect) {
  // Get the operation code {index} by getting only the first five bits
  uint8_t read = Serial::read();

  switch (read)
  {
    // Toggle enabling servo
    // 0
    case '0':                          
      servo.enable = !servo.enable;
      return;

    // Rotate servo to the right
    // 1
    case '1':
      servo.increment(-1);
      return;

    // Rotate servo to the left
    // 2
    case '2':
      servo.increment(1);
      return;

    // Stop rotation of servo
    // 3
    case '3':
      servo.done = true;
      return;

    // Turn manual mode off
    // 4
    case '4':
      servo.manual = false;
      return;
  }

  uint8_t index = read & 0x0f;
  if (index > NUM_OF_COMMANDS - 1) {
    return;
  }
  uint8_t command;

  // For every motor on the shield, execute the appropriate command
  for (uint8_t i = 0, shift = LB; i < 3; i++, shift+= 2) {
    command = (rxCommands[index] >> shift) & 0b11;
    motors[i].run(command);
  }

  // For the weapon, make sure to update values of manuallyOn
  switch (command)
  {
  case IGNORE:
    return;
  case RELEASE:
    manuallyOn = false;
    break;
  default:
    manuallyOn = true;
    break;
  }
}

void putOff(int direction, uint8_t* reach, bool* reachIncrement) {

  // Turn on the fan
  weapon.run(FORWARD);

  switch (*reachIncrement) {
    case true:
      if (*reach == 60) {
        *reachIncrement = false;
        return;
      }
      *reach += REACH_DIFF;
      break;
    default:
      if (*reach == 0) {
        *reachIncrement = true;
        return;
      }
      *reach -= REACH_DIFF;
      break;
  }

  // Turn the servo to direction + reach
  servo.write(direction + *reach);
}



bool servoOn = false;

ISR(TIMER1_COMPA_vect) {

  switch (servoOn)
  {
  case false:
    TCNT1 = 0;
    OCR1A = TCNT1 + servo.ticks;
    if (servo.enable){
      PORTB |= _BV(servo.pin);
      servoOn = true;
    }
    break;
  case true:
    PORTB &= ~_BV(servo.pin);
    if (TCNT1 + 4 < usToTicks(REFRESH_INTERVAL))
      OCR1A = usToTicks(REFRESH_INTERVAL);
    else
      OCR1A = TCNT1 + 4;
    servoOn = false;
    break;
  }

}