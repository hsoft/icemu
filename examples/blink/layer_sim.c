#include "layer.h"
#include "../../capi/icemu.h"
#include "../../capi/avr/attiny.h"

void pinhigh()
{
    icemu_pinset(ICEMU_PINB1, true);
}

void pinlow()
{
    icemu_pinset(ICEMU_PINB1, false);
}

void pinoutputmode()
{
}

void _delay_ms(unsigned int ms)
{
    icemu_delay_ms(ms);
}
