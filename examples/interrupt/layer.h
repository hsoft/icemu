#pragma once
#ifndef SIMULATION
#include <avr/io.h>
#include <avr/interrupt.h>
#else
#include "../../capi/icemu.h"
#include "../../capi/avr/attiny.h"
#endif

#include <stdbool.h>

void pinset(bool high);
bool pinishigh();
void pininputmode();
void pinoutputmode();
