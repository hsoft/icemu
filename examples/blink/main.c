#ifndef SIMULATION
#include <avr/io.h>
#include <util/delay.h>
#endif

#include "layer.h"

#define BLINK_DELAY_MS 1000

void setup()
{
    pinoutputmode();
}

void loop()
{
    pinhigh();
    _delay_ms(BLINK_DELAY_MS);
    pinlow();
    _delay_ms(BLINK_DELAY_MS);
}

#ifndef SIMULATION
int main()
{
    setup();
    while(1) {
        loop();
    }
}
#endif
