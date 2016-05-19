CC_MCU = atmega8

AVRDUDE_MCU = m8
PROGRAMMER = usbasp

all: pull compile objhex

compile:
	avr-g++ -O3 -Wall -std=c++11 -funsigned-char -mmcu=$(CC_MCU) main.cpp -o obj.o

objhex:
	avr-objcopy -j .text -j .data -O ihex obj.o out.hex

pull:
	git pull

deploy:
	avrdude -p $(AVRDUDE_MCU) -c $(PROGRAMMER) -U flash:w:out.hex

clean:
	rm *.o *.hex *~
