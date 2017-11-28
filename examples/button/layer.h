#pragma once
#ifndef SIMULATION
#include <avr/io.h>
#include <util/delay.h>
#endif

#include <stdbool.h>

void pinset(bool high);
bool pinishigh();
void pininputmode();
void pinoutputmode();
#ifdef SIMULATION
void _delay_ms(unsigned int ms);
#endif
