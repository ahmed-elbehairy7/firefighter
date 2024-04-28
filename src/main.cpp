/*
////        Defining some used stuff        
*/

// Defining F_CPU, rx & tx ubrr and baud rate
#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/(long(16) * BAUD) -1

#define bitIsSet(macro, bit) ((macro & _BV(bit)))
#define bitIsClear(macro, bit) (!(macro & _BV(bit)))
#define loopUntilBitIsSet(macro, bit) do { } while (bitIsSet(macro, bit))
#define loopUntilBitIsClear(macro, bit) do { } while (bitIsClear(macro, bit))

/*
////        including liberaries        
*/
#define __AVR_ATmega328P__
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


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
  DDRB = _BV(DDB5) | _BV(DDB4) | _BV(DDB3) | _BV(DDB2) | _BV(DDB1);
  // (*(volatile char *)0x53) |= 7;

  /*
    lines until ADMUX = ... probably not abstracted by arduino
    analogReference(INTERNAL);
    __LOGIC__:: the first line disables analog ports 
    so it doesn't consume power, and the second one 
    sets the analogReference to AREF pin and do 
    some configuration for us
  */
  /* Arduino abstracted:
    Serial.begin(9600);
  */
  // Set baud rate
  UBRR0H =(MYUBRR >> 8);
  UBRR0L = MYUBRR;
  // Enable receiver and transmitter
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  // Set frame format: 8data, 1stop bit
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);


  // Enabling global interrupt
  SREG |= _BV(SREG_I);

  // Enabling port b interrupt
  PCICR |= _BV(PCIE0);

  // Enabling pin change interrupt for pin 8
  PCMSK0 |= _BV(PCINT0);

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
    ;
  }
  

}


ISR(PCINT0_vect) {

  /* Arduino abstracted:
    char read = digitalread(8);
    if (!read) {
    digitalWrite(8, LOW);
    return false;
    }
  */

  // return if no fire detected from sensor digital input
  PORTB |= _BV(PB0);       // set a pullover on pin 0 to read it by pinb
  // insert a nop
  _NOP();
  // if sensor detects fire

  // loopUntilBitIsClear(UCSR0A, UDRE0);

  // // // Put data into buffer, sends the data
  // UDR0 = PINB;
  
  if (bit_is_set(PINB, PINB0)) {
    // make sure fan is on
    PORTB |= _BV(PB1);
  }
  else {
    // make sure fan is off
    PORTB &= ~_BV(PB1);
  }

  // /* Arduino abstracted:
  //   char analogRead = analogRead(A0);
  // */
  // // Read the sensor analog data
  // PRR &= ~_BV(PRADC);       // turning on adc from prr
  // ADMUX &= ~(_BV(MUX0) | _BV(MUX1) | _BV(MUX2) | _BV(MUX3));              // Setting the channel input to adc0 right before convertion
  // ADCSRA |= _BV(ADEN) | _BV(ADSC);       //ADEN enables ADC and ADSC starts a conversion 
  // loopUntilBitIsSet(ADCSRA, ADSC);      // waiting for the read to complete
  // short analogRead = ADCH;          //Reading the value of the adc data register
  // ADCSRA &= ~_BV(ADEN);       //Resetting the ADEN bit to zero again

  // /* Arduino abstracted:
  //   Serial.print(read);
  // */
  // // Sending data by tx 
  // // Wait for empty transmit buffer
  // loopUntilBitIsClear(UCSR0A, UDRE0);

  // // Put data into buffer, sends the data
  // UDR0 = analogRead;

  // // if fire in range, turn on the fans, stop the car and put it off
  // if (analogRead <= RANGE) {
  //   /* Arduino abstracted:
  //     digitalWrite(8, HIGH);
  //   */
  //   // turning the fan on
  //   PORTB = _BV(PB1);
  //   return;
  // }

  // /* Arduino abstracted:
  //   digitalWrite(13, HIGH);
  //   digitalWrite(12, LOW);
  //   digitalWrite(11, HIGH);
  //   digitalWrite(10, LOW);
  // */
  // // else, go towards the fire
  // PORTB = _BV(PB5) | _BV(PB3);
  // return;
}


ISR (USART_RX_vect) {
  
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

   //error checking
    // if (UCSR0A & _BV(FE0) | _BV(DOR0) | _BV(UPE0)) {
    //     PORTB = 0;
    //     continue;
    // }

    // forward  h => 104 => 01101000    ||   ( => 040 => 00101000
    // backward T => 084 => 01010100
    // right    ` => 096 => 01100000
    // left     H => 072 => 01001000
    // stop     @ => 064 => 01000000
    // fan on   B => 066 => 01000010
    PORTB = UDR0 & (~(1 << 6)); // modify the wheels movement as got from the serial

    /* Arduino abstracted:
      while( Serial.available()) {
        Serial.read();
      }
    */
    // empty the registery from unwanted data
    while ( bitIsSet(UCSR0A, RXC0) ) {volatile char dummy = UDR0;}
}
