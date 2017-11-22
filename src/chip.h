#pragma once
#include "pin.h"

#define MAX_PINS_PER_CHIP 64

typedef void (*PinChangeFunc)(Chip *, Pin *);

struct Chip {
    void *logical_unit;
    PinChangeFunc pin_change_func;
    Pin * pins[MAX_PINS_PER_CHIP];
};

Pin* icemu_chip_addpin(Chip *chip, char *code, bool output, bool low_means_high);
