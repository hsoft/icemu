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

void mmcu_pinhigh();
void mmcu_pinlow();
void mmcu_pinoutputmode();

#ifdef SIMULATION
void _delay_us(unsigned int us);
#endif
