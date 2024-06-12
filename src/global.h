/*
////        Defining constants        
*/

#define F_CPU 16000000UL
#define FORWARD 1
#define BACKWARD 2
#define RELEASE 3
#define __AVR_ATmega328P__


/*
////        Defining bit functions
*/

#define bitIsSet(macro, bit) ((macro & _BV(bit)))
#define bitIsClear(macro, bit) (!(macro & _BV(bit)))
#define loopUntilBitIsSet(macro, bit) do { } while (bitIsSet(macro, bit))
#define loopUntilBitIsClear(macro, bit) do { } while (bitIsClear(macro, bit))



/*
////        Including liberaries
*/

#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>