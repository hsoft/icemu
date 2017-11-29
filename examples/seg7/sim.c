#include <stdio.h>
#include <unistd.h>
#include "layer.h"

static ICeChip mcu;
static ICeChip sr;
static ICeChip seg;
static ICePin *pb0;
static ICePin *pb1;
static ICePin *pb2;

/* main.c decl */
void setup();
void loop();

static ICePin* pinid(PinID pin)
{
    switch (pin) {
        case PinB0: return pb0;
        case PinB1: return pb1;
        case PinB2: return pb2;
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
    char * segorder[] = {"F", "G", "E", "D", "C", "B", "A", "DP"};
    int i;
    ShiftRegister *srl;

    icemu_ATtiny_init(&mcu);
    icemu_SN74HC595_init(&sr);
    icemu_seg7_init(&seg);
    srl = (ShiftRegister *)sr.logical_unit;

    pb0 = icemu_chip_getpin(&mcu, "PB0");
    pb1 = icemu_chip_getpin(&mcu, "PB1");
    pb2 = icemu_chip_getpin(&mcu, "PB2");

    icemu_pin_wireto(pb0, icemu_chip_getpin(&sr, "SRCLK"));
    icemu_pin_wireto(pb1, icemu_chip_getpin(&sr, "SER"));
    icemu_pin_wireto(pb2, icemu_chip_getpin(&sr, "RCLK"));

    for (i = 0; i < 8; i++) {
        icemu_pin_wireto(srl->outputs.pins[i], icemu_chip_getpin(&seg, segorder[i]));
    }
    setup();
    icemu_sim_init(50, loop);
    icemu_sim_add_chip(&mcu);
    icemu_sim_add_chip(&seg);
    icemu_ui_add_element("MCU:", &mcu);
    icemu_ui_add_element("SR:", &sr);
    icemu_ui_add_element("Seg7:", &seg);
    icemu_sim_run();
    return 0;
}
