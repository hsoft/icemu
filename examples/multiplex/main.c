#include <stdbool.h>
#include "layer.h"

static volatile unsigned char n;

#ifndef SIMULATION
ISR(TIMER0_COMPA_vect)
#else
void myinterrupt()
#endif
{
    n++;
    pinset(PinB0, n & 0x1);
    pinset(PinB1, n & 0x2);
    pinset(PinB2, n & 0x4);
}

int main (void)
{
#ifndef SIMULATION
    sei();
#endif

    pinoutputmode(PinB0);
    pinoutputmode(PinB1);
    pinoutputmode(PinB2);
    n = 0;

    set_timer0_target(1000UL * 5UL); // every 5ms
    set_timer0_mode_to_interrupt();

#ifdef SIMULATION
    while(1) {
        icemu_process_messages();
        if (icemu_check_timer(ICEMU_TIMER0)) {
            myinterrupt();
        }
    }
#endif
}
