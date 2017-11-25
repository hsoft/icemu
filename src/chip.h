#pragma once
#include <sys/time.h>
#include "pin.h"

typedef struct {
    uint16_t width;
    uint16_t height;
    char contents[4096]; // should be enough for any chip
} ChipAsciiArt;

typedef void (*PinChangeFunc)(Pin *);
typedef void (*AsciiArtFunc)(Chip *, ChipAsciiArt *);
typedef void (*ElapseFunc)(Chip *, time_t);

struct Chip {
    void *logical_unit;
    PinChangeFunc pin_change_func;
    AsciiArtFunc asciiart_func;
    ElapseFunc elapse_func;
    PinList pins;
};

void icemu_chip_init(Chip *chip, void *logical_unit, PinChangeFunc pin_change_func, uint8_t pin_count);
Pin* icemu_chip_addpin(Chip *chip, const char *code, bool output);
void icemu_chip_addpins(Chip *chip, PinList *dst_pinlist, const char **codes, bool output);
Pin* icemu_chip_getpin(Chip *chip, const char *code);
void icemu_chip_elapse(Chip *chip, time_t usecs);
