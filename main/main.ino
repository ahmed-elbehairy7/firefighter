uint8_t back = 40;         //the value when the fire is near

// defining used functions
bool handleFire();
void setWheels(uint8_t n);

void setup() {
  //for every pin of the output pins
  for (uint8_t i = 9; i < 14; i++) {
    pinMode(i, OUTPUT);            //set the pinmode to output
  } 

  pinMode(A0, INPUT);       //sensor analog pin
  pinMode(8, INPUT);        //sensor digital pin

  Serial.begin(9600);         //start serial monitor
 
}
 
void loop() {

  delay(200);             //delay 200 ms

  //if fire handled
  if (handleFire()) {
    // ignore whatever user sent by bluetooth and continue the loop
    return;
  }

  //if nothing sent by bluetooth
  if (!Serial.available()) {
    setWheels(0);         //stop the car
    return;             //end the loop
  }
    
  setWheels(Serial.read());                   //modify the wheels movement as got from the serial
}

bool handleFire() {

  // return if no fire detected
  if (!digitalRead(8)) {
    digitalWrite(9, 0);
    return false;
  }

  //read the sensor
  int read = analogRead(A0);
  Serial.print("sensor: ");
  Serial.println(read);

  //if fire is so high go back   
  if (read <= back) {
    setWheels(42);
  }
  // if fire in range, stop the car, turn on the fans and put it off
  else if (read <= back + 4) {
    setWheels(0);
    digitalWrite(9, 1);
  }
  // else, go towards the fire
  else {
    setWheels(37);
  }
  // return true to indicate that a fire is detected
  return true;
}

void setWheels(uint8_t n) {

  /*
    Binary digits for movement:
      forward 0101
      backward 1010
      right 0001
      left 0100
      right reverse 1000
      left reverse 0010
      stop 0000

      13 left forward; 
      12 left reverse;
      11 right forward;
      10 right reverse;
  */

  //for every wheel pin
  digitalWrite(13, n & 1);
  digitalWrite(12, n & 2);
  digitalWrite(11, n & 4);
  digitalWrite(10, n & 8);


  //same functionality with for loop

  // // {left forward, left reverse, right forward, right reverse}
  // uint8_t wheels[nWheels] = {13, 12, 11, 10};

  // //the binary digits to compare with serial value
  // uint8_t binaries[nWheels] = {1, 2, 4, 8};

  // for (uint8_t i = 0; i < nWheels; i++) {
  //   digitalWrite(wheels[i], n & binaries[i]);             //write the corresponding current to it
  // } 
}