#pragma once
#include <sys/time.h>
#include "pin.h"
#include "pinlist.h"

typedef struct {
    uint16_t width;
    uint16_t height;
    char contents[4096]; // should be enough for any chip
} ICeChipAsciiArt;

typedef void (*AsciiArtFunc)(const ICeChip *, ICeChipAsciiArt *);
typedef void (*ElapseFunc)(ICeChip *, time_t);

struct ICeChip {
    void *logical_unit;
    PinChangeFunc pin_change_func;
    AsciiArtFunc asciiart_func;
    ElapseFunc elapse_func;
    ICePinList pins;
};

void icemu_chip_init(ICeChip *chip, void *logical_unit, PinChangeFunc pin_change_func, uint8_t pin_count);
ICePin* icemu_chip_addpin(ICeChip *chip, const char *code, bool output);
void icemu_chip_addpins(ICeChip *chip, ICePinList *dst_pinlist, const char **codes, bool output);
ICePin* icemu_chip_getpin(const ICeChip *chip, const char *code);
void icemu_chip_elapse(ICeChip *chip, time_t usecs);
