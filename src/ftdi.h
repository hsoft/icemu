#pragma once
#include "pinlist.h"

typedef struct {
    int handle;
    ICePinList inputs;
} ICeFTDI;

void icemu_FT232H_init(ICeChip *chip);


