#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pin.h"
#include "chip.h"

void icemu_pin_set(Pin *pin, bool high)
{
    if (high != pin->high) {
        pin->high = high;
        pin->chip->pin_change_func(pin);
    }
}

void icemu_pinlist_init(PinList *pinlist, uint8_t count)
{
    pinlist->count = count;
    pinlist->pins = malloc(sizeof(Pin *) * count);
    memset(pinlist->pins, 0, sizeof(Pin *) * count);
}
