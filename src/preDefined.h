/*
////        Defining commonly used stuff        
*/

// Convert an address into macro
#define MACRO(mem_addr) (*(volatile char*)(mem_addr))
// Shift bits
#define _BIT(bit) (1 << bit)
// Check if bit is set to 1
#define bit_is_set(macro, bv) (macro & bv)
// Check if bit is cleared
#define bit_is_clear(macro, bv) (!(macro & bv))


/*
////        Defining Registers        
*/

// The Port B Input Pins Address
#define PINB MACRO(0x23)
#define PINB0 0

// The Port B Data Direction Register
#define DDRB MACRO(0x24)
#define DDB5 5
#define DDB4 4
#define DDB3 3
#define DDB2 2
#define DDB1 1
#define DDB0 0

// The Port B Data Register
#define PORTB MACRO(0x25)
#define PB5 5
#define PB4 4
#define PB3 3
#define PB2 2
#define PB1 1
#define PB0 0

// Power Reduction Register
#define PRR MACRO(0x64)
#define PRADC 0     // Power Reduction ADC

// The ADC Data Register H
#define ADCH MACRO(0x79)

// ADC Control and Status Register A
#define ADCSRA MACRO(0x7A)
#define ADEN 7      // ADC Enable
#define ADSC 6      // ADC Start Conversion

// ADC Multiplexer Selection Register
#define ADMUX MACRO(0x7C)
#define ADLAR 5     // ADC Left Adjust Result

// Digital Input Disable Register 0
#define DIDR0 MACRO(0x7E)
#define ADC5D 5
#define ADC4D 4
#define ADC3D 3
#define ADC2D 2
#define ADC1D 1
#define ADC0D 0

// USART Control and Status Register 0 A
#define UCSR0A MACRO(0xC0)
#define RXC0 7      // USART Receive Complete
#define TXC0 6      // USART Transmit Complete
#define UDRE0 5     // USART Data Register Empty

// USART Control and Status Register 0 B
#define UCSR0B MACRO(0xC1)
#define RXEN0 4     // Receiver Enable n
#define TXEN0 3     // Transmitter Enable n

// USART Control and Status Register n C
#define UCSR0C MACRO(0xC2)
#define USBS0 3     // Stop Bit Select
#define UCSZ01 2    // Character size
#define UCSZ00 1    // Character Size

// USART Baud Rate Register L
#define UBRR0L MACRO(0xC4)

// USART Baud Rate Register H
#define UBRR0H MACRO(0xC5)

// USART I/O Data Register 0
#define UDR0 MACRO(0xC6)