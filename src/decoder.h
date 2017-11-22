#pragma once
#include "pin.h"

typedef struct {
    PinList inputs;
    PinList outputs;
} Decoder;

void icemu_SN74HC138_init(Chip *chip);

