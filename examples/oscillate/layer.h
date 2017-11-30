#pragma once
#include <stdbool.h>

#ifdef SIMULATION
#include "../../src/icemu.h"
#endif


void osc_setup_interrupt();
bool osc_set_timer0_target(unsigned long usecs);
void osc_set_timer0_mode_to_interrupt();
