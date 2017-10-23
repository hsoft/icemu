#include "layer.h"

#ifndef SIMULATION
ISR(INT0_vect)
#else
void int0_interrupt()
#endif
{
    pinset(pinishigh());
}

int main (void)
{
#ifndef SIMULATION
    sei();
#endif

    pininputmode();
    pinoutputmode();

#ifdef SIMULATION
    while(1) {
        icemu_process_messages();
        if (icemu_check_interrupt(ICEMU_INT0) == ICEMU_INT0) {
            int0_interrupt();
            icemu_end_interrupt();
        }
    }
#endif
}
