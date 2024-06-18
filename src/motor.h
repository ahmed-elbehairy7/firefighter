#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

#define FREQ _BV(CS01)
#define MOTORLATCH _BV(DDB4)
#define MOTORDATA _BV(DDB0)
#define MOTORENABLE _BV(DDD7)
#define MOTORCLK _BV(DDD4)
#define SPEED 0xff
#define IGNORE 0
#define FORWARD 1
#define BACKWARD 2
#define RELEASE 3

#define LATCH_AND_DATA_DRR DDRB
#define LATCH_AND_DATA_PORT PORTB
#define ENABLE_AND_CLK_DRR DDRD
#define ENABLE_AND_CLK_PORT PORTD

class Motor
{
private:
    uint8_t motorNum;
    void latch_tx();
public:
    Motor(uint8_t motorNum);
    void run(uint8_t direction);
};

#endif