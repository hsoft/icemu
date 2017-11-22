#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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

