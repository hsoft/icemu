#include "layer.h"

#ifndef SIMULATION
ISR(PCINT0_vect)
#else
void int0_interrupt()
#endif
{
    pinset(pinishigh());
}

int main (void)
{

    pininputmode();
    pinoutputmode();
    setup_interrupt();

    // apparently, we *need* an infinite loop for the interrupt to launch
    while(1) {
#ifdef SIMULATION
        icemu_process_messages();
        if (icemu_check_interrupt(ICEMU_INT0) == ICEMU_INT0) {
            int0_interrupt();
            icemu_end_interrupt();
        }
#endif
    }
}
