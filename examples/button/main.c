#include "layer.h"

#define POLL_DELAY_MS 100

void setup()
{
    pininputmode();
    pinoutputmode();
}

void loop()
{
    pinset(pinishigh());
    _delay_ms(POLL_DELAY_MS);
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
