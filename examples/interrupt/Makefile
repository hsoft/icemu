PROGNAME = interrupt
MCU ?= attiny45
F_CPU ?= 1000000UL
CC = avr-gcc
CFLAGS = -Os -Wall -DF_CPU=$(F_CPU) -mmcu=$(MCU) -c
LDFLAGS = -mmcu=$(MCU)
OBJS ?= main.o layer.o

AVRDUDEMCU = t45
AVRDUDEARGS = -c usbtiny -P usb 

SIM_OBJS = main.o sim.o
ICEMU_LIB = ../../libicemu.a

# Patterns

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Rules

.PHONY: send clean

all: $(PROGNAME).hex
	@echo Done!

send: $(PROGNAME).hex
	avrdude $(AVRDUDEARGS) -p $(AVRDUDEMCU) -U flash:w:$(PROGNAME).hex

clean:
	rm -f $(OBJS) $(PROGNAME).hex $(PROGNAME).bin

$(PROGNAME).bin: $(OBJS)
	$(CC) $(LDFLAGS) $+ -o $@

$(PROGNAME).hex: $(PROGNAME).bin
	avr-objcopy -O ihex -R .eeprom $< $@

# Simulation

.PHONY: sim

$(ICEMU_LIB):
	gmake -C ../..

$(PROGNAME): $(SIM_OBJS) $(ICEMU_LIB)
	$(CC) `pkg-config --libs ncurses tinfo` -L../.. $(SIM_OBJS) -o $@ -licemu

sim: CC = cc
sim: CFLAGS = -DSIMULATION -Wall `pkg-config --cflags ncurses` -c
sim: $(PROGNAME)
	@echo Done!
