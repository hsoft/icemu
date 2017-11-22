#pragma once
#include <stdbool.h>

typedef struct Chip Chip;

typedef struct Pin {
    Chip *chip;
    char *code;
    bool output;
    bool low_means_high;
    bool high;
} Pin;

void icemu_pin_set(Pin *pin, bool high);
