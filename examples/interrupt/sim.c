#include <stdio.h>
#include <unistd.h>
#include "layer.h"
#include "../../src/icemu.h"

static ICeChip mcu;
static ICePin *pb0;
static ICePin *pb1;

/* main.c decl */
void setup();
void int0_interrupt();

/* layer impl */
void setup_interrupt()
{
    icemu_mcu_add_interrupt(&mcu, pb0, INTERRUPT_ON_BOTH, int0_interrupt);
}

void pinset(bool high)
{
    icemu_pin_set(pb1, high);
}

bool pinishigh()
{
    return pb0->high;
}

void pininputmode()
{
    pb0->output = false;
}

void pinoutputmode()
{
    pb1->output = true;
}

/* Simulation */

void toggle_button()
{
    icemu_pin_set(pb0, !pb0->high);
}

int main()
{
    icemu_ATtiny_init(&mcu);
    pb0 = icemu_chip_getpin(&mcu, "PB0");
    pb1 = icemu_chip_getpin(&mcu, "PB1");
    setup();
    icemu_sim_init(50, NULL);
    icemu_sim_add_action('b', "Toggle (b)utton", toggle_button);
    icemu_ui_add_element("MCU:", &mcu);
    icemu_sim_run();
    return 0;
}
