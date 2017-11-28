#include "layer.h"

void pinhigh(PinID pin)
{
    sbi(PORTB, pin & 0b111);
}

void pinlow(PinID pin)
{
    cbi(PORTB, pin & 0b111);
}

void pinset(PinID pin, bool high)
{
    if (high) {
        pinhigh(pin);
    } else {
        pinlow(pin);
    }
}

void pinoutputmode(PinID pin)
{
    sbi(DDRB, pin & 0b111);
}

