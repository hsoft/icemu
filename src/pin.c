#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "pin.h"
#include "chip.h"

void icemu_pin_set(Pin *pin, bool high)
{
    if (high != pin->high) {
        pin->high = high;
        pin->chip->pin_change_func(pin);
    }
}

void icemu_pinlist_init(PinList *pinlist, uint8_t capacity)
{
    pinlist->capacity = capacity;
    pinlist->count = 0;
    pinlist->pins = malloc(sizeof(Pin *) * capacity);
    memset(pinlist->pins, 0, sizeof(Pin *) * capacity);
}

void icemu_pinlist_add(PinList *pinlist, Pin *pin)
{
    assert(pinlist->count < pinlist->capacity);

    pinlist->pins[pinlist->count] = pin;
    pinlist->count++;
}
