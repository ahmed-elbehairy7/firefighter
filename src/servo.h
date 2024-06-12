#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

#define MIN_PULSE_WIDTH       544     // the shortest pulse sent to a servo  
#define MAX_PULSE_WIDTH      2400     // the longest pulse sent to a servo 
#define TRIM_DURATION 2               // compensation ticks to trim adjust for digitalWrite delays
#define SERVO_DELAY 5
#define DEFAULT_PULSE_WIDTH 1500
#define REFRESH_INTERVAL    20000     // minimum time to refresh servos in microseconds 
#define clockCyclesPerMicrosecond (F_CPU / 1000000L)
#define usToTicks(_us) ((_us * clockCyclesPerMicrosecond ) / 8)             // Converts microseconds to ticks
#define ticksToUs(_ticks) ((_ticks * 8) / clockCyclesPerMicrosecond)        // From ticks to microseconds
#define DEFAULT_TICKS usToTicks(DEFAULT_PULSE_WIDTH)
#define map(x, in_min, in_max, out_min, out_max) (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


class Servo
{
private:    
    uint8_t servoAngle;
    void writeMicroSeconds(int value);
public:
    uint8_t pin;
    int ticks;
    Servo(uint8_t pin);
    void write(uint8_t angle);
};

#endif