/*
////        including liberaries        
*/
// defining f_cpu for delay
#ifndef F_CPU
  #define F_CPU 16000000UL // Clock Speed
#endif

#include <util/delay.h>
#include "./preDefined.h"

/*
////        Defining some used variables        
*/

// Defining rx tx ubrr and baud rate
#define BAUD 9600
#define MYUBRR F_CPU/(long(16) * BAUD) -1

// defining when to tell fire is close
#define range 10


/*
////        Defining used functions        
*/
bool handleFire();

/*
////        Main job        
*/

int main() {

  /*
  ////        Setup        
  */


  //for every pin of the output pins
  // left forward, left reverse, right forward, right reverse, fan, sensor DO
  // ports:  PB5 PB4 PB3 PB2 PB1 PB0
  // pins:   13  12  11  10  09  08
  // binary: 1   1   1   1   1   0   = 62 in decimal

  /* Arduino abstracted:
    pinMode(13, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(9, OUTPUT);
    PinMode(8, INPUT);
  */
  DDRB = _BIT(DDB5) | _BIT(DDB4) | _BIT(DDB3) | _BIT(DDB2) | _BIT(DDB1);

  /*
    the first line probably not abstracted by arduino
    analogReference(INTERNAL);
    __LOGIC__:: the first line disables analog ports 
    so it doesn't consume power, and the second one 
    sets the analogReference to AREF pin and do 
    some configuration for us
  */
  // configuring settings for analog input
  DIDR0 = _BIT(ADC5D) | _BIT(ADC4D) | _BIT(ADC3D) | _BIT(ADC2D) | _BIT(ADC1D) | _BIT(ADC0D);   //disabling digital inputs for unused adc pins
  PRR |= _BIT(PRADC);       // turning off adc from prr
  ADMUX = _BIT(ADLAR); // setting the Adc data register to be left adjusted by the ADLAR pin, Setting also the voltage reference to aref by setting REFS0 and REF1 to 0

  /* Arduino abstracted:
    Serial.begin(9600);
  */
  // Set baud rate
  UBRR0H =(MYUBRR >> 8);
  UBRR0L = MYUBRR;
  // Enable receiver and transmitter
  UCSR0B = _BIT(RXEN0) | _BIT(TXEN0);
  // Set frame format: 8data, 2stop bit
  UCSR0C = _BIT(USBS0 ) | _BIT(UCSZ01) | _BIT(UCSZ00);


  /*
  ////        Loop        
  */


  while (true)
  {
    /* Arduino abstracted
      delay(500);
    */
    _delay_ms(500);

    // handle the fire
    if (handleFire()) {
      continue;
    }

    /* Arduino abstracted
      Serial.available();
    */
    // checking if nothing recieved from serial by checking RXC0 bit from the UCSR0A register "Serial.available();"
    if (bit_is_clear(UCSR0A, _BIT(RXC0))) {
      continue;
    }

    /* Arduino abstracted

      char rxVal = Serial.read() & ~(1 << 6);
      digitalWrite(13, rxVal & (1 << 5));
      digitalWrite(12, rxVal & (1 << 4));
      digitalWrite(11, rxVal & (1 << 3));
      digitalWrite(10, rxVal & (1 << 2));
      digitalWrite(9, rxVal & (1 << 1));

    __NOTE__:: 
    there's no abstracted version of this because
    there's no digitalWrite for 5 pins at the same time. So,
    what this function does is setting the pins 13->8 to high
    and low based on the binary digit got from the serial or
    to be more percise RX
    this code includes some bitwise operations, search for 
    that term if you don't know the function of &, ~, <<.
    __LOGIC__::
    So, first you have to know what's actually HIGH and LOW
    simply they're the result of this code:
    #define HIGH 1
    #define LOW 0
    1 means true and low means false as  a boolean data type,
    second, how we know which pin to set to high is by reading 
    the binary digits of the recieved value of Serial.read()
    for example the character F will have the binary digits of
    01000110 and the character 'h' will be 01101000. more on 
    that here: https://ascii-code.com
    so, pin 13 will always be the 6th digit from right of 
    the recieved value by the serial (RX), and the expression
    "rxVal & (1 << 5)" gives us this bit whether if it's 1 or
    zero
    */


    // forward  h => 104 => 01101000    ||   ( => 040 => 00101000
    // backward T => 084 => 01010100
    // right    ` => 096 => 01100000
    // left     H => 072 => 01001000
    // stop     @ => 064 => 01000000
    // fan on   B => 066   => 01000010
    PORTB = UDR0;//    &(~(1 << 6)); // modify the wheels movement as got from the serial

    /* Arduino abstracted:
      while( Serial.available()) {
        Serial.read();
      }
    */
    // empty the registery from unwanted data
    while ( bit_is_set(UCSR0A, _BIT(RXC0)) ) {volatile char dummy = UDR0;}

    // Sending data by tx 
    // Wait for empty transmit buffer
    while ( bit_is_clear( UCSR0A, _BIT(UDRE0 )) ) {}
    
    // Put data into buffer, sends the data
    UDR0 = PORTB;

    UCSR0A |= _BIT(TXC0);
  }
  

}

bool handleFire() {

  /* Arduino abstracted:
    char read = digitalread(8);
    if (!read) {
    digitalWrite(8, LOW);
    return false;
    }
  */
  // return if no fire detected from sensor digital input
  PORTB |= _BIT(PB0);       // set a pullover on pin 0 to read it by pinb

  // insert a nop
  (0x00 << 0);
  // if no electricity in pin number 8
  if (bit_is_clear(PINB, _BIT(PINB0))) {
    // make sure fan is off
    PORTB &= ~_BIT(PB1);
    return false;
  }

  /* Arduino abstracted:
    char read = analogRead(A0);
  */
  // Read the sensor analog data
  PRR &= ~_BIT(PRADC);       // turning on adc from prr
  DIDR0 = ~_BIT(ADC0D);       // getting A0 ready for input
  ADCSRA |= _BIT(ADEN) | _BIT(ADSC);       //ADEN enables ADC and ADSC starts a conversion 
  while(bit_is_set(ADCSRA, ADSC));      // waiting for the read to complete
  unsigned char read = ADCH;          //Reading the value of the adc data register
  ADCSRA &= ~_BIT(ADEN);       //Resetting the ADEN bit to zero again

  /* Arduino abstracted:
    Serial.print(read);
  */
  // Sending data by tx 
  // Wait for empty transmit buffer
  while ( bit_is_clear( UCSR0A, _BIT(UDRE0 )) )

  // Put data into buffer, sends the data
  UDR0 = read;

  // if fire in range, turn on the fans, stop the car and put it off
  if (read <= range) {
    /* Arduino abstracted:
      digitalWrite(8, HIGH);
    */
    // turning the fan on
    PORTB = _BIT(PB1);
    return false;
  }

  /* Arduino abstracted:
    digitalWrite(13, HIGH);
    digitalWrite(12, LOW);
    digitalWrite(11, HIGH);
    digitalWrite(10, LOW);
  */
  // else, go towards the fire
  PORTB = _BIT(PB5) | _BIT(PB3);
  return true;

}