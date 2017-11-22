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

bool icemu_pin_check_if_changed(Pin *pin);
void icemu_pin_set(Pin *pin, bool high);
