#include "layer.h"

#define POLL_DELAY_MS 100

int main (void)
{
    pininputmode();
    pinoutputmode();

    while(1) {
        pinset(pinishigh());
        _delay_ms(POLL_DELAY_MS);
    }
}
