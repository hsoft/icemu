#include <stdio.h>
#include <unistd.h>
#include "layer.h"
#include "../../src/icemu.h"

static Chip mcu;
static Chip dec;
static Chip lm;
static Pin *pb0;
static Pin *pb1;
static Pin *pb2;

/* main.c decl */
void setup();
void myinterrupt();

static Pin* pinid(PinID pin)
{
    switch (pin) {
        case PinB0: return pb0;
        case PinB1: return pb1;
        case PinB2: return pb2;
        default: return pb0;
    }
}


void pinset(PinID pin, bool high)
{
    icemu_pin_set(pinid(pin), high);
}

void pinoutputmode(PinID pin)
{
    pinid(pin)->output = true;
}

bool set_timer0_target(unsigned long usecs)
{
    icemu_mcu_add_timer(&mcu, usecs, myinterrupt);
    return true;
}

void set_timer0_mode_to_interrupt()
{
}

/* Simulation */

int main()
{
    Decoder *dec_lu;
    int i;

    icemu_ATtiny_init(&mcu);
    icemu_SN74HC138_init(&dec);
    icemu_ledmatrix_init(&lm, 4, 2);
    dec_lu = (Decoder *)dec.logical_unit;

    pb0 = icemu_chip_getpin(&mcu, "PB0");
    pb1 = icemu_chip_getpin(&mcu, "PB1");
    pb2 = icemu_chip_getpin(&mcu, "PB2");

    icemu_pin_wireto(pb0, icemu_chip_getpin(&dec, "A"));
    icemu_pin_wireto(pb1, icemu_chip_getpin(&dec, "B"));
    icemu_pin_wireto(pb2, icemu_chip_getpin(&dec, "C"));

    for (i = 0; i < 8; i++) {
        icemu_pin_wireto(dec_lu->outputs.pins[i], lm.pins.pins[i]);
    }
    setup();
    icemu_sim_init(50, NULL);
    icemu_ui_add_element("MCU:", &mcu);
    icemu_ui_add_element("LED:", &lm);
    icemu_sim_add_chip(&mcu);
    icemu_sim_add_chip(&lm);
    icemu_sim_run();
}
