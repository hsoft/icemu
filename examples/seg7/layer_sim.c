#include "layer.h"
#include "../../capi/icemu.h"
#include "../../capi/avr/attiny.h"

static unsigned char pinid(Pin pin)
{
    switch (pin) {
        case PinB0: return ICEMU_PINB0;
        case PinB1: return ICEMU_PINB1;
        case PinB2: return ICEMU_PINB2;
        case PinB3: return ICEMU_PINB3;
        case PinB4: return ICEMU_PINB4;
        case PinB5: return ICEMU_PINB5;
        default: return ICEMU_PINB0;
    }
}

void pinhigh(Pin pin)
{
    pinset(pin, true);
}

void pinlow(Pin pin)
{
    pinset(pin, false);
}

void pinset(Pin pin, bool high)
{
    icemu_pinset(pinid(pin), high);
}

void pinoutputmode(Pin pin)
{
    icemu_pinsetmode(pinid(pin), true);
}

void _delay_us(unsigned int us)
{
    icemu_delay_us(us);
}

void _delay_ms(unsigned int ms)
{
    icemu_delay_ms(ms);
}
