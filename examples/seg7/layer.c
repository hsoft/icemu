#include "layer.h"

void pinhigh(Pin pin)
{
    sbi(PORTB, pin & 0b111);
}

void pinlow(Pin pin)
{
    cbi(PORTB, pin & 0b111);
}

void pinset(Pin pin, bool high)
{
    if (high) {
        pinhigh(pin);
    } else {
        pinlow(pin);
    }
}

void pinoutputmode(Pin pin)
{
    sbi(DDRB, pin & 0b111);
}

