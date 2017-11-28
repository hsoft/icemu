#pragma once
#include "pin.h"
#include "pinlist.h"

typedef struct {
    uint8_t value;
    ICePin *clock;
    ICePinList inputs;
    ICePinList outputs;
    ICePinList enable_pins;
} BinaryCounter;

void icemu_SN74F161AN_init(ICeChip *chip);

