#pragma once
#include "pinlist.h"

typedef struct {
    ICePinList inputs;
} ICeFTDI;

// Returns whether we would connect to the device
bool icemu_FT232H_init(ICeChip *chip);


