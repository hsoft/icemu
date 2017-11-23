#pragma once
#include "pin.h"

typedef void (*PinChangeFunc)(Pin *);

struct Chip {
    void *logical_unit;
    PinChangeFunc pin_change_func;
    PinList pins;
};

void icemu_chip_init(Chip *chip, void *logical_unit, PinChangeFunc pin_change_func, uint8_t pin_count);
Pin* icemu_chip_addpin(Chip *chip, char *code, bool output, bool low_means_high);
Pin* icemu_chip_getpin(Chip *chip, char *code);
uint8_t icemu_chip_asciiart_width(Chip *chip);
uint8_t icemu_chip_asciiart_height(Chip *chip);
uint16_t icemu_chip_asciiart_len(Chip *chip);
void icemu_chip_asciiart(Chip *chip, char *dst);
