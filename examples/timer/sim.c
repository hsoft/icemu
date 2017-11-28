#include <stdio.h>
#include <unistd.h>
#include "layer.h"
#include "../../src/icemu.h"

static Chip mcu;
static ICePin *pb1;

/* main.c decl */
void setup();
void myinterrupt();

/* layer impl */
void pinset(bool high)
{
    icemu_pin_set(pb1, high);
}

void pinoutputmode()
{
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
    icemu_ATtiny_init(&mcu);
    pb1 = icemu_chip_getpin(&mcu, "PB1");
    setup();
    icemu_sim_init(50, NULL);
    icemu_ui_add_element("MCU:", &mcu);
    icemu_sim_add_chip(&mcu);
    icemu_sim_run();
    return 0;
}
