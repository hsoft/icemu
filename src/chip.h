#pragma once
#include "pin.h"

typedef struct {
    uint16_t width;
    uint16_t height;
    char contents[4096]; // should be enough for any chip
} ChipAsciiArt;

typedef void (*PinChangeFunc)(Pin *);
typedef void (*AsciiArtFunc)(Chip *, ChipAsciiArt *);
typedef void (*ElapseFunc)(Chip *, uint32_t);

struct Chip {
    void *logical_unit;
    PinChangeFunc pin_change_func;
    AsciiArtFunc asciiart_func;
    ElapseFunc elapse_func;
    PinList pins;
};

void icemu_chip_init(Chip *chip, void *logical_unit, PinChangeFunc pin_change_func, uint8_t pin_count);
Pin* icemu_chip_addpin(Chip *chip, const char *code, bool output, bool low_means_high);
Pin* icemu_chip_getpin(Chip *chip, char *code);
void icemu_chip_tick(Chip *chip);
