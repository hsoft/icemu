#pragma once
#include "chip.h"
#include "pin.h"

#define MAX_INTERRUPTS 100

typedef void (*InterruptFunc)();

typedef struct {
    uint64_t epoch; // usecs
    uint64_t last_tick;
    // index of the interrupt func must be the same as the index of the pin the interrupt
    // is bound to.
    InterruptFunc interrupts[MAX_INTERRUPTS];
} MCU;

void icemu_mcu_add_interrupt(Chip *chip, Pin *pin, InterruptFunc interrupt);
void icemu_ATtiny_init(Chip *chip);
