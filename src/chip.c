#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "chip.h"
#include "util.h"

#define MAX_CODE_LEN 10

void icemu_chip_init(Chip *chip, void *logical_unit, PinChangeFunc pin_change_func, uint8_t pin_count)
{
    chip->logical_unit = logical_unit;
    chip->pin_change_func = pin_change_func;
    icemu_pinlist_init(&chip->pins, pin_count);
}

Pin* icemu_chip_addpin(Chip *chip, char *code, bool output, bool low_means_high)
{
    Pin *result;

    result = icemu_pin_new(chip, code, output, low_means_high);
    icemu_pinlist_add(&chip->pins, result);
    return result;
}

Pin* icemu_chip_getpin(Chip *chip, char *code)
{
    uint8_t i;

    for (i = 0; i < chip->pins.count; i++) {
        if (strncmp(code, chip->pins.pins[i]->code, MAX_CODE_LEN) == 0) {
            return chip->pins.pins[i];
        }
    }
    return NULL;
}
