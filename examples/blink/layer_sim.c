#include "layer.h"
#include "../../capi/icemu.h"
#include "../../capi/avr/attiny.h"

void pinhigh()
{
    icemu_pinhigh(ICEMU_PINB1);
}

void pinlow()
{
    icemu_pinlow(ICEMU_PINB1);
}

void pinoutputmode()
{
}

void _delay_ms(unsigned int ms)
{
    icemu_delay_ms(ms);
}
