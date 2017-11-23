#pragma once
#ifndef SIMULATION
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

#include <stdbool.h>

void setup_interrupt();
void pinset(bool high);
bool pinishigh();
void pininputmode();
void pinoutputmode();
