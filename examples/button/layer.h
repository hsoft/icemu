#pragma once
#ifndef SIMULATION
#include <avr/io.h>
#include <util/delay.h>
#else
#include "../../capi/icemu.h"
#define _delay_ms icemu_delay_ms
#endif

#include <stdbool.h>

void pinset(bool high);
bool pinishigh();
void pininputmode();
void pinoutputmode();
