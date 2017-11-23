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
    usleep(ms * 1000);
}

/* Simulation */

void toggle_button()
{
    icemu_pin_set(pb0, !pb0->high);
}

int main()
{
    bool running = true;

    icemu_ATtiny_init(&mcu);
    pb0 = icemu_chip_getpin(&mcu, "PB0");
    pb1 = icemu_chip_getpin(&mcu, "PB1");
    setup();
    icemu_ui_init();
    icemu_ui_add_label("q - Quit");
    icemu_ui_add_label("b - Toggle button");
    icemu_ui_add_element("MCU:", &mcu);
    icemu_ui_refresh();
    while (running) {
        loop();
        switch (icemu_ui_refresh()) {
            case 'b': toggle_button();
                 break;
            case 'q': running = false;
                 break;
        }
    }
    icemu_ui_deinit();
}
