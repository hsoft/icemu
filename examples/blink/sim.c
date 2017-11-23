#include <stdio.h>
#include <unistd.h>
#include "layer.h"
#include "../../src/icemu.h"

static Chip mcu;
static Pin *pb1;
static char asciiart[4096];

/* main.c decl */
void setup();
void loop();

/* layer impl */
void pinhigh()
{
    icemu_pin_set(pb1, true);
    icemu_chip_asciiart(&mcu, asciiart);
    printf("%s\n", asciiart);
}

void pinlow()
{
    icemu_pin_set(pb1, false);
    icemu_chip_asciiart(&mcu, asciiart);
    printf("%s\n", asciiart);
}

void pinoutputmode()
{
}

void _delay_ms(unsigned int ms)
{
    usleep(ms * 1000);
}

/* Simulation */

int main()
{
    icemu_ATtiny_init(&mcu);
    pb1 = icemu_chip_getpin(&mcu, "PB1");
    setup();
    while (1) {
        loop();
    }
}
