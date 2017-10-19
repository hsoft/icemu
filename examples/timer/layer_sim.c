#include "layer.h"
#include "../../capi/icemu.h"
#include "../../capi/avr/attiny.h"

void pinset(bool high)
{
    icemu_pinset(ICEMU_PINB1, high);
}

void pinoutputmode()
{
}

bool set_timer0_target(unsigned long ticks)
{
    return icemu_start_timer(ICEMU_TIMER0, ticks);
}

void set_timer0_mode_to_interrupt()
{
}

bool sim_process_events()
{
    icemu_tick_time();
    return icemu_check_timer(ICEMU_TIMER0);
}
