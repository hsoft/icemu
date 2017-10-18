#ifndef SIMULATION
#include <avr/io.h>
#include <util/delay.h>
#endif

#include "layer.h"

#define BLINK_DELAY_MS 1000

int main (void)
{
    pinoutputmode();

    while(1) {
        pinhigh();
        _delay_ms(BLINK_DELAY_MS);
        pinlow();
        _delay_ms(BLINK_DELAY_MS);
    }
}
