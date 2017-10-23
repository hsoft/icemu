#include "layer.h"

static unsigned char pinid(Pin pin)
{
    switch (pin) {
        case PinB0: return ICEMU_PINB0;
        case PinB1: return ICEMU_PINB1;
        case PinB2: return ICEMU_PINB2;
        default: return ICEMU_PINB0;
    }
}

void pinset(Pin pin, bool high)
{
    icemu_pinset(pinid(pin), high);
}

void pinoutputmode(Pin pin)
{
    icemu_pinsetmode(pinid(pin), true);
}

bool set_timer0_target(unsigned long usecs)
{
    return icemu_start_timer(ICEMU_TIMER0, usecs);
}

void set_timer0_mode_to_interrupt()
{
}

