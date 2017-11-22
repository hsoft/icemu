#include <stdlib.h>

#include "util.h"

uint8_t icemu_util_pincount(Pin **pins, uint8_t max)
{
    uint8_t i;

    for (i = 0; i < max; i++) {
        if (pins[i] == NULL) {
            break;
        }
    }
    return i;
}
