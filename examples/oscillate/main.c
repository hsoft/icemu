#include <stdint.h>
#ifndef SIMULATION
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

#include "layer.h"

static volatile uint32_t freq; // freq for 1ms
static volatile uint32_t counter;

#ifndef SIMULATION
ISR(PCINT0_vect)
#else
void int0_interrupt()
#endif
{
    counter++;
}

#ifndef SIMULATION
ISR(TIMER0_COMPA_vect)
#else
void timer0_interrupt()
#endif
{
    freq = counter;
    counter = 0;
#ifdef SIMULATION
    icemu_sim_set_debug_value("freq", freq);
#endif
}

void setup()
{
#ifndef SIMULATION
    sei();
#endif

    set_timer0_target(1000UL * 100); // every 100ms
    set_timer0_mode_to_interrupt();
    setup_interrupt();
}

#ifndef SIMULATION
int main (void)
{
    setup();
    while(1);
}
#endif
