#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

class Motor
{
private:
    /* data */
public:
    Motor(uint8_t motorNum);
    void run(uint8_t direction);
};

#endif