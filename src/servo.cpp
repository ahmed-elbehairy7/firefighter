#include "global.h"
#include "servo.h"
#include "serial.h"

Servo::Servo(uint8_t pinNum) {

    // Set the pin to output
    this->pin = pinNum - 8;
    DDRB |= _BV(pin);

    // Initializing the timer
    TCCR1A = 0;                 // normal counting mode
    TCCR1B = _BV(CS11);         // Set prescaler of 8
    TCNT1 = 0;                  // Clear the timer count

    TIFR1 |= _BV(OCF1A);        // Clear any pending interrupts
    TIMSK1 |= _BV(OCIE1A);      // enable the output compare interrupt
}

void Servo::write(uint8_t angle) {

    Serial::print(angle);
    Serial::print('\n');

    if (angle == this->servoAngle) {
        return;
    }

    char increment = this->servoAngle > angle ? -1 : 1;

    // For in in the range start to finish
    for (; this->servoAngle != angle; this->servoAngle += increment) {
        
        // Write the angle of the servo as the current value
        writeMicroSeconds(map(this->servoAngle, 0, 180, MIN_PULSE_WIDTH * 4, MAX_PULSE_WIDTH * 4));

        // Delay 5 seconds
        _delay_ms(SERVO_DELAY);
    }
}

void Servo::writeMicroSeconds(int value) {
    value = value - TRIM_DURATION;
    value = usToTicks(value);           // convert to ticks after compensating for interrupt overhead
    uint8_t oldSREG = SREG;
    cli();
    this->ticks = value;
    SREG = oldSREG;
}