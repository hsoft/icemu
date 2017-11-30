#include <stdio.h>
#include <unistd.h>
#include "layer.h"
#include "../../src/icemu.h"

static ICeChip mcu;
static ICeChip ftdi;
static ICePin *pb1;

/* main.c decl */
void setup();
void loop();

/* layer impl */
void pinhigh()
{
    icemu_pin_set(pb1, true);
}

void pinlow()
{
    icemu_pin_set(pb1, false);
}

void pinoutputmode()
{
    pb1->output = true;
}

void _delay_ms(unsigned int ms)
{
    icemu_sim_delay(ms * 1000);
}

/* Simulation */

int main()
{
    icemu_ATtiny_init(&mcu);
    // 500ms duration because of our 500ms delay;
    icemu_mcu_set_runloop(&mcu, loop, 500 * 1000UL);
    pb1 = icemu_chip_getpin(&mcu, "PB1");
    icemu_FT232H_init(&ftdi);
    icemu_pin_wireto(pb1, icemu_chip_getpin(&ftdi, "D0"));
    icemu_sim_init();
    setup();
    icemu_ui_add_element("MCU:", &mcu);
    icemu_ui_add_element("FTDI:", &ftdi);
    icemu_sim_run();
    return 0;
}
