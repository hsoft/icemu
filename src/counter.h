#pragma once
#include "pin.h"

typedef struct {
    uint8_t value;
    Pin *clock;
    PinList inputs;
    PinList outputs;
    PinList enable_pins;
} BinaryCounter;

void icemu_SN74F161AN_init(Chip *chip);

