// defining used functions
bool handleFire();

void setup() {
  //for every pin of the output pins
  // left forward, left reverse, right forward, right reverse, fan, sensor DO
  // ports:  PB5 PB4 PB3 PB2 PB1 PB0
  // pins:   13  12  11  10  09  08
  // binary: 1   1   1   1   1   0   = 62 in decimal
  
  DDRB = (1 << PB5) | (1 << PB4) | (1 << PB3) | (1 << PB2) | (1 << PB1);
  PORTB = (1 << PB0);       // set a pullover on pin 0 to read it by pinb
  ADMUX |= (3 << 6);        // setting the reference for analog read as the internal reference

  Serial.begin(9600);         //start serial monitor
 
}
 
void loop() {

  delay(500);
  //if fire handled
  if (handleFire()) {
    // ignore whatever user sent by bluetooth and continue the loop
    return;
  }

  //if not recieved command from bluetooth
  if (!Serial.available()) {
    PORTB = 0;
    return;
  }
  // forward 4 => 40
  // backward * => 20
  // right 0 => 32
  // left " => 8
  PORTB = (Serial.read() & 31) << 1;                   //modify the wheels movement as got from the serial
  Serial.read();                                // get rid of the line feed got from the serial	
}

bool handleFire() {
	
  // return if no fire detected from sensor digital input
  if (!PINB0) {
    // make sure fan is off
    PORTB &= ~(1 << 1);
    // return false to tell the arduino to complete it's job
    return false;
  }

  //read the sensor analog data
  ADCSRA |= (3 << 6);       //starting an analog read
  while(bit_is_set(ADCSRA, ADSC));      // waiting for the read to complete
  short read = ADCL | (ADCH << 8);      // setting the variable read to the value read from A0
  Serial.println(read);

  // if fire in range, turn on the fans, stop the car and put it off
  if (read <= 30) {
    PORTB = 2;
  }
  // else, go towards the fire
  else {
    PORTB = 40;
  }
  // return true to indicate that a fire is detected
  return true;
}