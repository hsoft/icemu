#include <stdbool.h>
#ifndef SIMULATION
#include <avr/io.h>
#include <util/delay.h>
#endif

#include "layer.h"

#define BLINK_DELAY_MS 500

static bool flag;

void setup()
{
    flag = false;
    pinoutputmode();
}

void loop()
{
    if (flag) {
        pinhigh();
    } else {
        pinlow();
    }
    flag = !flag;
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
