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

void setup()
{
#ifndef SIMULATION
    sei();
#endif

    pin_high = false;
    pinoutputmode();

    set_timer0_target(1000UL * 500UL); // every 0.5s
    set_timer0_mode_to_interrupt();
}

#ifndef SIMULATION
int main (void)
{
    setup();
    while(1);
}
#endif
