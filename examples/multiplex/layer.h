#pragma once
#include <stdbool.h>
#ifndef SIMULATION
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

typedef enum {
    PinB0 = 0b01000,
    PinB1 = 0b01001,
    PinB2 = 0b01010,
} PinID;

void pinset(PinID pin, bool high);
void pinoutputmode(PinID pin);

bool set_timer0_target(unsigned long usecs);
void set_timer0_mode_to_interrupt();
