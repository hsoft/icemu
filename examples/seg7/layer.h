#pragma once
#include <stdbool.h>

#ifndef SIMULATION
#include <avr/io.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
#endif

typedef enum {
    PinB0 = 0b01000,
    PinB1 = 0b01001,
    PinB2 = 0b01010,
    PinB3 = 0b01011,
    PinB4 = 0b01100,
    PinB5 = 0b01101,
    PinB6 = 0b01110,
    PinB7 = 0b01111,
} PinID;

void pinhigh(PinID pin);
void pinlow(PinID pin);
void pinset(PinID pin, bool high);
void pinoutputmode(PinID pin);

#ifdef SIMULATION
void _delay_us(unsigned int us);
void _delay_ms(unsigned int ms);
#endif
