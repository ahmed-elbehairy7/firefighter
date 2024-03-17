char t;

// {left forward, left reverse, right forward, right reverse, left fan, middle fan, right fan}
int pins[7] = {13, 12, 11, 10, 9, 8, 7};

void setup() {

  for (int i = 0; i < 7; i++) {
    pinMode(pins[i], OUTPUT);
  }
  Serial.begin(9600);
 
}
 
void loop() {

  //delay 100 ms
  delay(100);

  if (!Serial.available()) {
    return;
  }
  
  //Read the value sent by bluetooth
  t = Serial.read();

  //print the value
  Serial.println(t);
  
  if(t == 'F'){            //move  forward(all motors rotate in forward direction)
    digitalWrite(13,HIGH);
    digitalWrite(11,HIGH);
  }
  
  else if(t == 'B'){      //move reverse (all  motors rotate in reverse direction)
    digitalWrite(12,HIGH);
    digitalWrite(10,HIGH);
  }
    
  else if(t == 'L'){      //turn right (left side motors rotate in forward direction,  right side motors doesn't rotate)
    digitalWrite(11,HIGH);
  }
  
  else  if(t == 'R'){      //turn left (right side motors rotate in forward direction, left  side motors doesn't rotate)
    digitalWrite(13,HIGH);
  }
  
  else if(t == 'S'){      //STOP (all motors stop)

    for (int i = 0; i < 4; i++) {
      digitalWrite(pins[i], LOW);
    }
  }
  
}