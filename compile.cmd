echo "Compiling main file"

avr-gcc -Os -mmcu=atmega328p ./src/main.cpp -o ./main.o
