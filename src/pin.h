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

typedef struct {
    uint8_t capacity;
    uint8_t count;
    Pin **pins;
} PinList;

bool icemu_pin_check_if_changed(Pin *pin);
void icemu_pin_set(Pin *pin, bool high);

void icemu_pinlist_init(PinList *pinlist, uint8_t capacity);
void icemu_pinlist_add(PinList *pinlist, Pin *pin);
