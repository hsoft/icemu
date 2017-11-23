#include "layer.h"

#ifndef SIMULATION
ISR(PCINT0_vect)
#else
void int0_interrupt()
#endif
{
    pinset(pinishigh());
}

void setup()
{
    pininputmode();
    pinoutputmode();
    setup_interrupt();
}

#ifndef SIMULATION
int main()
{
    setup();
    // apparently, we *need* an infinite loop for the interrupt to launch
    while(1) {
    }
}
#endif
