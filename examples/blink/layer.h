#pragma once

#ifndef SIMULATION
#include <avr/io.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
#endif

void pinhigh();
void pinlow();
void pinoutputmode();

#ifdef SIMULATION
void _delay_ms(unsigned int ms);
#endif
