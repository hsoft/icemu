#include <unistd.h>
#include "layer.h"
#include "../../src/icemu.h"

static ICeChip mcu;
static ICePin *pb0;
static ICePin *pb1;

/* main.c decl */
void setup();
void loop();

/* layer impl */
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
}

void pinoutputmode()
{
}

void _delay_ms(unsigned int ms)
{
    icemu_sim_delay(ms * 1000);
}

/* Simulation */

void toggle_button()
{
    icemu_pin_set(pb0, !pb0->high);
}

int main()
{
    icemu_sim_init();
    icemu_ATtiny_init(&mcu);
    // 100 * 1000 duration because of our 100ms polling delay
    icemu_mcu_set_runloop(&mcu, loop, 100 * 1000UL);
    pb0 = icemu_chip_getpin(&mcu, "PB0");
    pb1 = icemu_chip_getpin(&mcu, "PB1");
    setup();
    icemu_sim_add_action('b', "Toggle (b)utton", toggle_button);
    icemu_ui_add_element("MCU:", &mcu);
    icemu_sim_run();
    return 0;
}
