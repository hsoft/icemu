#pragma once
#include <stdbool.h>

#ifdef SIMULATION
#include "../../src/icemu.h"
#endif


void setup_interrupt();
bool set_timer0_target(unsigned long usecs);
void set_timer0_mode_to_interrupt();
