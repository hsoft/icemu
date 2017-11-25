#include <unistd.h>
#include "layer.h"
#include "../../src/icemu.h"

static Chip mcu;
static Pin *pb0;
static Pin *pb1;

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
    icemu_ATtiny_init(&mcu);
    pb0 = icemu_chip_getpin(&mcu, "PB0");
    pb1 = icemu_chip_getpin(&mcu, "PB1");
    setup();
    icemu_sim_init(50, loop);
    icemu_sim_add_action('b', "Toggle (b)utton", toggle_button);
    icemu_ui_add_element("MCU:", &mcu);
    icemu_sim_run();
    return 0;
}
