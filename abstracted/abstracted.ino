
// defining when to tell fire is close
#define range 10

char rxVal;
char read;

// Defining used functions        
bool handleFire();

/*
////        Setup        
*/

void setup() {
    pinMode(13, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(8, INPUT);

    analogReference(INTERNAL);

    Serial.begin(9600);
}

  /*
  ////        Loop        
  */
void loop() {
    delay(500);

    // handle the fire
    if (handleFire()) {
        return;
    }

    // checking if anything is recieved
    if (!Serial.available()) {
        return;
    }

    // forward  h => 104 => 01101000    ||   ( => 040 => 00101000
    // backward T => 084 => 01010100
    // right    ` => 096 => 01100000
    // left     H => 072 => 01001000
    // stop     @ => 064 => 01000000
    // fan on   B => 066   => 01000010

    rxVal = Serial.read() & ~(1 << 6);
    digitalWrite(13, rxVal & (1 << 5));
    digitalWrite(12, rxVal & (1 << 4));
    digitalWrite(11, rxVal & (1 << 3));
    digitalWrite(10, rxVal & (1 << 2));
    digitalWrite(9, rxVal & (1 << 1));

    while( Serial.available()) {
        Serial.read();
    }

    Serial.print(rxVal);
  }

bool handleFire() {

    read = digitalRead(8);
    if (!read) {
    digitalWrite(8, LOW);
    return false;
    }

    read = analogRead(A0);

    Serial.print(read);

  // if fire in range, turn on the fans, stop the car and put it off
    if (read <= range) {
        digitalWrite(8, HIGH);
        return false;
    }
    
    // else, go towards the fire
    digitalWrite(13, HIGH);
    digitalWrite(12, LOW);
    digitalWrite(11, HIGH);
    digitalWrite(10, LOW);
    return true;
}