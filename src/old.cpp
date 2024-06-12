/*
////        Defining some used stuff        
*/

// Defining F_CPU, rx & tx ubrr and baud rate
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
#define LB 0
#define RB 2
#define WB 4

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

#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <AFMotor.h>

#define ARDUINO_ARCH_AVR

#include <Servo.h>



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

AF_DCMotor motors[3] = {left, right, weapon};


/*
////        Prototypes
*/

void handleRX();
void handleFile();
void putOff(int direction);
void servoWrite(int angle);

/*
////        Main job        
*/

int main() {

  /*
  ////        Setup        
  */

  //Set speeds of motors

  for (char i = 0; i < 2; i++) {
    motors[i].setSpeed(SPEED);
  }
  servo.attach(SERVO);


  // Start the serial monitor
  // Set baud rate for the serial monitor
  UBRR0H =(MYUBRR >> 8);
  UBRR0L = MYUBRR;
  // Enable receiver and transmitter
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  // Set frame format: 8data, 1stop bit
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);


  // Enabling global interrupt
  SREG |= _BV(SREG_I);

  // Enabling rx interrupt
  UCSR0B |= _BV(RXCIE0);

  /*
  //// sleep
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
  ////        Loop        
  */

  while (true)
  {
    handleFile();
    handleRX();
  }
  

}


// ISR (USART_RX_vect) {
  
//     //error checking
//     if (UCSR0A & _BV(FE0) | _BV(DOR0) | _BV(UPE0)) {
//         PORTB = 0;
//         continue;
//     }

//     while ( bitIsSet(UCSR0A, RXC0) ) {volatile char dummy = UDR0;}
// }



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
    motors[i].run(rxCommands[index][i]);
  }

  // For the weapon, make sure to update values of manuallyOn
  switch (rxCommands[index][2]) {
      case FORWARD:
        manuallyOn = true;
        break;
      case RELEASE:
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
  // for (char sensor = LEFT_SENSOR, start = LEFT_START; sensor <= RIGHT_SENSOR; sensor++, start-= 60) {

  //   // if fire detected from that senosr
  //   if (!digitalRead(sensor)) {

  //     // put off that fire then return
  //     putOff(start);
  //     return;
  //   }
  // }

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
    _delay_ms(SERVO_DELAY * 1000);
  }
}
