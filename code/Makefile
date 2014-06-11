# AVR-GCC Makefile
PROJECT=display
SOURCES=main.c serial.c
CC=avr-gcc
OBJCOPY=avr-objcopy
MMCU=attiny2313

CFLAGS=-mmcu=$(MMCU) -Wall -Os


$(PROJECT).hex: $(PROJECT).out
	$(OBJCOPY) -j .text -O ihex $(PROJECT).out $(PROJECT).hex

$(PROJECT).out: $(SOURCES) 
	$(CC) $(CFLAGS) -I./ -o $(PROJECT).out  $(SOURCES)

program: $(PROJECT).hex
# fuses: external xtal, don't divide clock by 8
	avrdude -c usbtiny -e -p t2313 -U flash:w:$(PROJECT).hex -U lfuse:w:0xcf:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
clean:
	rm -f $(PROJECT).o
	rm -f $(PROJECT).out
	rm -f $(PROJECT).map
	rm -f $(PROJECT).hex
