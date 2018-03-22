#include <stdio.h>
#include <unistd.h>
#include "layer.h"

static ICeChip mcu;
static ICeChip dec;
static ICeChip seg;
static ICePin *pb0;
static ICePin *pb1;
static ICePin *pb2;
static ICePin *pb3;

/* main.c decl */
void setup();
void loop();

static ICePin* pinid(PinID pin)
{
    switch (pin) {
        case PinB0: return pb0;
        case PinB1: return pb1;
        case PinB2: return pb2;
        case PinB3: return pb3;
        default: return pb0;
    }
}

void pinhigh(PinID pin)
{
    pinset(pin, true);
}

void pinlow(PinID pin)
{
    pinset(pin, false);
}

void pinset(PinID pin, bool high)
{
    icemu_pin_set(pinid(pin), high);
}

void pinoutputmode(PinID pin)
{
    pinid(pin)->output = true;
}

void _delay_us(unsigned int us)
{
    icemu_sim_delay(us);
}

void _delay_ms(unsigned int ms)
{
    icemu_sim_delay(ms * 1000);
}

/* Simulation */

int main()
{
    icemu_sim_init();
    icemu_ATtiny_init(&mcu);
    icemu_mcu_set_runloop(&mcu, loop, 1000UL * 1000UL);
    icemu_SN7447A_init(&dec);
    icemu_seg7_init(&seg);

    pb0 = icemu_chip_getpin(&mcu, "PB0");
    pb1 = icemu_chip_getpin(&mcu, "PB1");
    pb2 = icemu_chip_getpin(&mcu, "PB2");
    pb3 = icemu_chip_getpin(&mcu, "PB3");

    icemu_pin_wireto(pb0, icemu_chip_getpin(&dec, "A"));
    icemu_pin_wireto(pb1, icemu_chip_getpin(&dec, "B"));
    icemu_pin_wireto(pb2, icemu_chip_getpin(&dec, "C"));
    icemu_pin_wireto(pb3, icemu_chip_getpin(&dec, "D"));

    icemu_displaydecoder_wireto_seg7(&dec, &seg);

    setup();
    icemu_ui_add_element("MCU:", &mcu);
    icemu_ui_add_element("DEC:", &dec);
    icemu_ui_add_element("Seg7:", &seg);
    icemu_sim_run();
    return 0;
}
