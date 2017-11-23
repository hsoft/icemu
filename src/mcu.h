#pragma once
#include "chip.h"

typedef struct {
    uint64_t epoch; // usecs
    uint64_t last_tick;
} MCU;

void icemu_ATtiny_init(Chip *chip);
