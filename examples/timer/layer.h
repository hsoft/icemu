#pragma once
#include <stdbool.h>

void pinset(bool high);
void pinoutputmode();

bool set_timer0_target(unsigned long usecs);
void set_timer0_mode_to_interrupt();

#ifdef SIMULATION
bool sim_process_events();
#endif
