#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "chip.h"

static uint8_t chip_pincount(Chip *chip)
{
    uint8_t i;

    for (i = 0; i < MAX_PINS_PER_CHIP; i++) {
        if (chip->pins[i] == NULL) {
            break;
        }
    }
    return i;
}

void icemu_chip_init(Chip *chip, void *logical_unit, PinChangeFunc pin_change_func)
{
    chip->logical_unit = logical_unit;
    chip->pin_change_func = pin_change_func;
    memset(chip->pins, 0, sizeof(Pin*) * MAX_PINS_PER_CHIP);
}

Pin* icemu_chip_addpin(Chip *chip, char *code, bool output, bool low_means_high)
{
    Pin *result;
    uint8_t index;

    result = (Pin *)malloc(sizeof(Pin));
    result->chip = chip;
    result->code = code;
    result->output = output;
    result->low_means_high = low_means_high;
    result->high = false;

    index = chip_pincount(chip);
    chip->pins[index] = result;
    return result;
}

