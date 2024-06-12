#include "global.h"
#include "serial.h"
#include <string.h>

#define BAUD 9600
#define MYUBRR F_CPU/(long(16) * BAUD) -1

void Serial::begin() {
    // Set baud rate
    UBRR0H =(MYUBRR >> 8);
    UBRR0L = MYUBRR;
    // Enable receiver and transmitter
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    // Set frame format: 8data, 1stop bit
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

uint8_t Serial::read() {
    return UDR0;
}

void Serial::print(char* text) {

    for (uint8_t i = 0, length = strlen(text); i < length; i++) {
        loopUntilBitIsClear(UCSR0A, UDRE0);
        UDR0 = text[i];
    }

}

void Serial::print(char c) {
    char character[2] = {c, 0};
    print(character);
}

void Serial::print(uint8_t n) {
    char chars[3] = {48, 48, 48};

    for (char i = 0; i < 3; i++){
        if (n <= 0) {
            break;
        }
        chars[i] = (n % 10) + 48;
        n = n / 10;
    }

    for (char i = 2; i >= 0; i--) {
        print(chars[i]);
    }
    
}

bool Serial::isAvailable() {
    return bitIsSet(UCSR0A, RXC0);
}