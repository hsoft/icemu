#pragma once
#include "pin.h"

typedef struct {
    PinList inputs;
    PinList outputs;
    PinList serial_pins;
    PinList enable_pins;
} Decoder;

void icemu_SN74HC138_init(Chip *chip);

