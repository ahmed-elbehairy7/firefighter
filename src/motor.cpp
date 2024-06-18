#include "global.h"
#include "motor.h"
#include "serial.h"

static uint8_t latchState{0};
static uint8_t MOTORS_A[4] = {2, 1, 5, 0};
static uint8_t MOTORS_B[4] = {3, 4, 7, 6};

void Motor::latch_tx() {
    
    LATCH_AND_DATA_PORT &= ~MOTORLATCH & ~MOTORDATA;

    for (uint8_t i = 0; i < 8; i++) {
        ENABLE_AND_CLK_PORT &= ~MOTORCLK;

        if (latchState & _BV(7-i)) 
            LATCH_AND_DATA_PORT |= MOTORDATA;
        else
            LATCH_AND_DATA_PORT &= ~MOTORDATA;

        ENABLE_AND_CLK_PORT |= MOTORCLK;
    }

    LATCH_AND_DATA_PORT |= MOTORLATCH;
}

Motor::Motor(uint8_t motorNum) {

    // set the motor num
    this->motorNum = motorNum;

    /*
    ////    Enable
    */
    [&]()
    {
        LATCH_AND_DATA_DRR |= MOTORLATCH | MOTORDATA;
        ENABLE_AND_CLK_DRR |= MOTORENABLE | MOTORCLK;

        latch_tx(); // Reset latch

        ENABLE_AND_CLK_PORT &= ~MOTORENABLE;

    }();

    uint8_t i = motorNum - 1;
    latchState &= ~_BV(MOTORS_A[i]) & ~_BV(MOTORS_B[i]);
    latch_tx();

    switch (motorNum)
    {
    case 1:
         // use PWM from timer2A on PB3 (Arduino pin #11)
        TCCR2A |= _BV(COM2A1) | _BV(WGM20) | _BV(WGM21); // fast PWM, turn on oc2a
        TCCR2B = 0x7;
        OCR2A = SPEED;
        DDRB |= _BV(DDB3);
        break;
    case 2:
        // use PWM from timer2A on PB3 (Arduino pin #11)
        TCCR2A |= _BV(COM2B1) | _BV(WGM20) | _BV(WGM21); // fast PWM, turn on oc2b
        TCCR2B = 0x7;
        OCR2B = SPEED;
        DDRD |= _BV(DDD3);
        break;
    case 3:
        // use PWM from timer0A / PD6 (pin 6)
        TCCR0A |= _BV(COM0A1) | _BV(WGM00) | _BV(WGM01); // fast PWM, turn on OC0A
        TCCR0B = 0x7;
        OCR0A = SPEED;
        DDRD |= _BV(DDD6);
        break;
    case 4:
        // use PWM from timer0B / PD5 (pin 5)
        TCCR0A |= _BV(COM0B1) | _BV(WGM00) | _BV(WGM01); // fast PWM, turn on oc0a
        TCCR0B = 0x7;
        OCR0B = SPEED;
        DDRD |= _BV(DDD5);
        break;
    }

    
}

void Motor::run(uint8_t direction) {

    if (direction < FORWARD) {
        return;
    }

    uint8_t i = this->motorNum - 1;
    switch (direction)
    {
    case FORWARD:
        latchState |= _BV(MOTORS_A[i]);
        latchState &= ~_BV(MOTORS_B[i]);
        break;
    case BACKWARD:
        latchState &= ~_BV(MOTORS_A[i]);
        latchState |= _BV(MOTORS_B[i]);
        break;
    case RELEASE:
        latchState &= ~_BV(MOTORS_A[i]);
        latchState &= ~_BV(MOTORS_B[i]);
        break;
    }
    latch_tx();
}