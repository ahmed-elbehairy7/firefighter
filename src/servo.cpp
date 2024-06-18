#include "global.h"
#include "servo.h"
#include "serial.h"
#include <avr/eeprom.h>

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

    servoAngle = eeprom_read_byte(EEPROM_SERVO_ANGLE_MEM);   // Read last angle
}

void Servo::write(uint8_t angle) {

    if (angle == this->servoAngle) {
        return;
    }

    char increment = this->servoAngle > angle ? -1 : 1;

    // For in in the range start to finish
    for (uint8_t i = this->servoAngle + increment, r = angle + increment; i != r;i += increment) {
        
        // Write the angle of the servo as the current value
        _write(i);

        // Delay 5 seconds
        _delay_ms(SERVO_DELAY);
    }

}

bool Servo::_write(uint8_t angle) {

    if (!this->enable) {
        return false;
    }

    bool returnValue = true;
    if (angle == START) {
        returnValue = false;
    }
    else if (angle > END) {
        angle = END;
        returnValue = false;
    }
    // Convert the angle to microseconds
    this->servoAngle = angle;
    eeprom_write_byte(EEPROM_SERVO_ANGLE_MEM, servoAngle);
    long value = angleToPulse((unsigned long)angle);

    // convert the microseconds to ticks
    value = value - TRIM_DURATION;
    value = usToTicks(value);           // convert to ticks after compensating for interrupt overhead
    
    // stop interrupts  while overwriting the value of ticks
    // uint8_t oldSREG = SREG;
    // cli();
    this->ticks = value;
    // SREG = oldSREG;

    return returnValue;
}

void Servo::increment(uint8_t increment) {
    sei();
    this->manual = true;
    this->done = false;
    while (_write(this->servoAngle + increment) && !this->done && this->manual)
    {
        _delay_ms(SERVO_DELAY);
    }
    this->done = true;
}
