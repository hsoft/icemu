#pragma once
#include "pin.h"
#include "pinlist.h"

typedef struct {
    ICePinList inputs;
    ICePinList outputs;
    ICePinList serial_pins;
    ICePinList enable_pins;
} Decoder;

void icemu_SN74HC138_init(ICeChip *chip);

