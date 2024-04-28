// including libraries
#include <AFMotor.h>

// defining constants
#define SENSOR 8
#define SPEED 255

// functions prototypes
void handleFire();
void moveCar();

// motors instances
AF_DCMotor fan(1);
AF_DCMotor left(2);
AF_DCMotor right(3);

void setup() {
  // put your setup code here, to run once:

  // Start serial with 9600 baud rate
  Serial.begin(9600);

  // initialize motor speeds
  fan.setSpeed(SPEED);
  left.setSpeed(SPEED);
  right.setSpeed(SPEED);
}

void loop() {
  // put your main code here, to run repeatedly:
  // function that handle the fire
  handleFire();

  // function to handle car movement
  moveCar();

}

void handleFire() {
  // read data from sensor
  // if fire detected
  if (digitalRead(SENSOR)) {
    // turn fan on
    fan.run(FORWARD);
  }
  else {
    // turn fan off
    fan.run(RELEASE);
  }
}

void moveCar() {

  // if there's nothing recieved from bluetooth sensor:
  //    exit;
  if (!Serial.available()) {
    return;
  }
  // else:
  //    move car according to command recieved by bluetooth

  switch (Serial.read()) {
    // forward case
    case 'F':
      left.run(FORWARD);
      right.run(FORWARD);
      break;

    // backward case
    case 'B':
      right.run(BACKWARD);
      left.run(BACKWARD);
      break;

    // left case 
    case 'L':
      right.run(FORWARD);
      left.run(RELEASE);
      break;
    
    // right case
    case 'R':
      left.run(FORWARD);
      right.run(RELEASE);
      break;

    // default case
    default:
      left.run(RELEASE);
      right.run(RELEASE);
      break;
  }

}
