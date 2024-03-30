echo "uploading main file..."

avr-gcc -Os -mmcu=atmega328p ./main.cpp -o ./main.o

avr-objcopy -O ihex -j .data -j .text ./main.o ./main.hex

avr-size -A ./main.hex

avrdude -v -V -p atmega328p -c arduino -P COM3 -b 115200 -D -U flash:w:./main.hex:i