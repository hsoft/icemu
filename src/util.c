#include <stdlib.h>

#include "util.h"

uint8_t icemu_util_chararray_count(const char **array)
{
    uint8_t result = 0;

    while (array[result] != NULL) {
        result++;
    }
    return result;
}

void icemu_util_set_binary_value(PinList *pinlist, uint16_t val)
{
    uint8_t i;

    for (i = 0; i < pinlist->count; i++) {
        icemu_pin_set(pinlist->pins[i], (val >> i) & 0x1);
    }
}

