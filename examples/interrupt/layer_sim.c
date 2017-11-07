#include "layer.h"

void setup_interrupt()
{
}

void pinset(bool high)
{
    icemu_pinset(ICEMU_PINB1, high);
}

bool pinishigh()
{
    return icemu_pinread(ICEMU_PINB0);
}

void pininputmode()
{
    icemu_pinsetmode(ICEMU_PINB0, false);
}

void pinoutputmode()
{
    icemu_pinsetmode(ICEMU_PINB1, true);
}
