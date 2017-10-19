#include <stdbool.h>
#ifndef SIMULATION
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

#include "layer.h"

#define BLINK_DELAY_MS 500

static volatile bool pin_high;

#ifndef SIMULATION
ISR(TIMER0_COMPA_vect)
#else
void myinterrupt()
#endif
{
    pin_high = !pin_high;
    pinset(pin_high);
}

int main (void)
{
#ifndef SIMULATION
    sei();
#endif

    pin_high = false;
    pinoutputmode();

    set_timer0_target(1000 * 500); // every 0.5s
    set_timer0_mode_to_interrupt();

#ifdef SIMULATION
    while(1) {
        if (sim_process_events()) {
            myinterrupt();
        }
    }
#endif
}
