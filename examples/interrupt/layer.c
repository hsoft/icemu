#include "layer.h"

#include <avr/io.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setup_interrupt()
{
    // enable Pin Change Interrupts
    sbi(GIMSK, PCIE);
    // Enable INT0 for interrupts
    sbi(PCMSK, PCINT0);
    sei();
}

void pinset(bool high)
{
    if (high) {
        sbi(PORTB, 1);
    } else {
        cbi(PORTB, 1);
    }
}

bool pinishigh()
{
    return (PINB & 0x1) != 0;
}

void pininputmode()
{
    cbi(DDRB, 0);
}

void pinoutputmode()
{
    sbi(DDRB, 1);
}

