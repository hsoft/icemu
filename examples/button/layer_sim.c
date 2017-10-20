#include "layer.h"
#include "../../capi/icemu.h"
#include "../../capi/avr/attiny.h"

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
}

void pinoutputmode()
{
}
