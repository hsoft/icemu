#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "chip.h"
#include "util.h"

void icemu_chip_init(Chip *chip, void *logical_unit, PinChangeFunc pin_change_func, uint8_t pin_count)
{
    chip->logical_unit = logical_unit;
    chip->pin_change_func = pin_change_func;
    icemu_pinlist_init(&chip->pins, pin_count);
}

Pin* icemu_chip_addpin(Chip *chip, char *code, bool output, bool low_means_high)
{
    Pin *result;

    result = (Pin *)malloc(sizeof(Pin));
    result->chip = chip;
    result->code = code;
    result->output = output;
    result->low_means_high = low_means_high;
    result->high = false;

    icemu_pinlist_add(&chip->pins, result);
    return result;
}

