#pragma once
#include "pinlist.h"

typedef struct {
    ICePinList inputs;
} ICeFTDI;

void icemu_FT232H_init(ICeChip *chip);


