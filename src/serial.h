#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

class Serial
{
public:
    Serial() {};
    static void begin();
    static void print(char* text);
    static void print(uint8_t n);
    static void print(char c);
    static uint8_t read();
    static bool isAvailable();
};

#endif