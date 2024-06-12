/*
////        Defining constants      
*/

#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/(long(16) * BAUD) -1
#define SPEED 255
#define LEFT_START 120
#define LEFT_SENSOR 15
#define RIGHT_SENSOR 17
#define CENTER 90
#define SERVO 9
#define SERVO_DELAY 9
#define REACH_DIFF 30


/*
////        Defining bit functions
*/

#define bitIsSet(macro, bit) ((macro & _BV(bit)))
#define bitIsClear(macro, bit) (!(macro & _BV(bit)))
#define loopUntilBitIsSet(macro, bit) do { } while (bitIsSet(macro, bit))
#define loopUntilBitIsClear(macro, bit) do { } while (bitIsClear(macro, bit))


/*
////        including liberaries        
*/

#include <AFMotor.h>
#include <avr/interrupt.h>
#include <Arduino.h>
#include <Servo.h>

/*
////        Initializing variables
*/

AF_DCMotor weapon(1);
AF_DCMotor left(2);
AF_DCMotor right(3);
Servo servo;
bool manuallyOn;
bool reachIncrement = true;
char reach;
int servoAngle;

/*
////        List of rx commands
*/
char rxCommands[8][3] = {
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

AF_DCMotor rxRecievers[3] = {left, right, weapon};


/*
////        Prototypes
*/

void handleRX();
void handleFire();
void putOff(int direction);
void servoWrite(int angle);


/*
////        Setup        
*/

void setup() {

  // Initializing speeds of motors
  for (char i = 0; i < 2; i++) {
    rxRecievers[i].setSpeed(SPEED);
  }
  servo.attach(SERVO);


  /* 
    Starting the serial 
  */

  // Set baud rate
  UBRR0H =(MYUBRR >> 8);
  UBRR0L = MYUBRR;
  // Enable receiver and transmitter
  UCSR0B = _BV(RXEN0);
  // Set frame format: 8data, 1stop bit
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

}


/*
////        Loop function
*/

void loop() {

  // The function responsible for handling bluetooth commands
  handleRX();

  // The function responsible for handling fire detection
  handleFire();
}


/*
////        handleRX funtion
*
*   if something found in the rx, it operates
*   according to it to move the car, and turn
*   the fan on and off
*
*/

void handleRX() {

  // If nothing is recived with rx ignore the whole function
  if (bitIsClear(UCSR0A, RXC0)) {
    return;
  }

  // Get the operation code {index} by getting only the first five bits
  char index = UDR0 & ~0b11100000;

  // For every motor on the shield, execute the appropriate command
  for (char i = 0; i < 3; i++) {
    rxRecievers[i].run(rxCommands[index][i]);
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


/*
////        handleFire funtion
*
*   if fire detected by one of the sensors,
*   it putoff the fire in the direction of 
*   that sensor
*
*/

void handleFire() {

  // For every sensor of the sensors
  for (char sensor = LEFT_SENSOR, start = LEFT_START; sensor <= RIGHT_SENSOR; sensor++, start-= 60) {

    // if fire detected from that senosr
    if (!digitalRead(sensor)) {

      // put off that fire then return
      putOff(start);
      return;
    }
  }

  // If no fire is detected and also the weapon not turned on manually
  if (!manuallyOn) {
    
    // make sure the weapon is not running
    weapon.run(RELEASE);

    // make the weapon look forward
    servoWrite(CENTER);

    // Return reach to zero
    reach = 0;
    
  }
}


/*
////        putOff funtion
*
*   takes direction argument from handleFire
*   function, turns on the fan and calls 
*   the writeServo function
*
*/

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
  servoWrite(direction + reach);

}


/*
////        servoWrite funtion
*
*   takes angle argument and rotates the
*   servo in a smooth way until it reach
*   it's destination
*
*/

void servoWrite(int angle) {
  if (angle == servoAngle) {
    return;
  }

  char increment = servoAngle > angle ? -1 : 1;

  // For in in the range start to finish
  for (int i = servoAngle; i != angle; i += increment) {
    
    // Write the angle of the servo as the current value
    servo.write(i);
    servoAngle = i;

    // Delay 5 seconds
    delay(SERVO_DELAY);
  }
}

