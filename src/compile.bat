@echo off

avr-gcc -Os -c -mmcu=atmega328p ./motor.cpp -o ./motor.o

avr-gcc -Os -c -mmcu=atmega328p ./main.cpp -o ./m.o

avr-gcc -Os -c -mmcu=atmega328p ./servo.cpp -o ./servo.o

avr-gcc -Os -c -mmcu=atmega328p ./serial.cpp -o ./serial.o

avr-gcc -Os -mmcu=atmega328p ./m.o ./motor.o ./servo.o ./serial.o -o ./main.o
	