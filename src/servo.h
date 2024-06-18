#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

#define MIN_PULSE_WIDTH       544     // the shortest pulse sent to a servo  
#define MAX_PULSE_WIDTH      2400     // the longest pulse sent to a servo 
#define TRIM_DURATION 0               // compensation ticks to trim adjust for digitalWrite delays
#define SERVO_DELAY 10
#define DEFAULT_PULSE_WIDTH 1500
#define REFRESH_INTERVAL    20000     // minimum time to refresh servos in microseconds 
#define clockCyclesPerMicrosecond (F_CPU / 1000000L)
#define usToTicks(_us) ((_us * clockCyclesPerMicrosecond ) / 8)             // Converts microseconds to ticks
#define ticksToUs(_ticks) ((_ticks * 8) / clockCyclesPerMicrosecond)        // From ticks to microseconds
#define DEFAULT_TICKS usToTicks(DEFAULT_PULSE_WIDTH)
#define angleToPulse(x) x * (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) / 180  + MIN_PULSE_WIDTH
#define EEPROM_SERVO_ANGLE_MEM 0


#define CENTER 90
#define START 0
#define END 180

class Servo
{
public:
    bool manual{true};
    bool done{true};
    bool enable {true};
    uint8_t servoAngle;
    uint8_t pin;
    long ticks {angleToPulse(CENTER)};
    Servo(uint8_t pin);
    void write(uint8_t angle);
    bool _write(uint8_t angle);
    void increment(uint8_t increment);
};

#endif