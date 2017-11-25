#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "pin.h"
#include "chip.h"

/* Private */
static void wire_propagate(PinList *wire)
{
    // We propagate to one or zero other pin on the wire. Our goal is to:
    // 1. propagate state to all wired pins before any pin_change_func() call.
    // 2. call pin_change_func() for each changed pin *after* propagation occured.

    bool wire_is_high = false;
    uint8_t i;
    Pin *p;

    for (i = 0; i < wire->count; i++) {
        p = wire->pins[i];
        if (p->output && p->high) {
            wire_is_high = true;
            break;
        }
    }
    for (i = 0; i < wire->count; i++) {
        p = wire->pins[i];
        if (!p->output && icemu_pin_set(p, wire_is_high)) {
            break;
        }
    }
}

/* Public */

Pin* icemu_pin_new(Chip *chip, const char *code, bool output, bool low_means_high)
{
    Pin *pin;
    pin = malloc(sizeof(Pin));
    icemu_pin_init(pin, chip, code, output, low_means_high);
    return pin;
}

void icemu_pin_init(Pin *pin, Chip *chip, const char *code, bool output, bool low_means_high)
{
    pin->chip = chip;
    pin->code = code;
    pin->output = output;
    pin->low_means_high = code[0] == '~';
    pin->high = false;
    pin->wire = NULL;
}

bool icemu_pin_set(Pin *pin, bool high)
{
    if (high != pin->high) {
        pin->high = high;
        if (pin->output && (pin->wire != NULL)) {
            wire_propagate(pin->wire);
        }
        if ((pin->chip != NULL) && (pin->chip->pin_change_func != NULL)) {
            pin->chip->pin_change_func(pin);
        }
        return true;
    }
    return false;
}

bool icemu_pin_enable(Pin *pin, bool enabled)
{
    bool high;

    high = pin->high != pin->low_means_high;
    return icemu_pin_set(pin, high);
}

bool icemu_pin_isenabled(Pin *pin)
{
    return pin->high != pin->low_means_high;
}

PinList* icemu_pinlist_new(uint8_t capacity)
{
    PinList *result;
    result = malloc(sizeof(PinList));
    icemu_pinlist_init(result, capacity);
    return result;
}

void icemu_pin_wireto(Pin *pin, Pin *other)
{
    if ((pin->wire == NULL) && (other->wire == NULL)) {
        pin->wire = icemu_pinlist_new(MAX_PINS_ON_A_WIRE);
        icemu_pinlist_add(pin->wire, pin);
        icemu_pinlist_add(pin->wire, other);
        other->wire = pin->wire;
    } else if (pin->wire != other->wire) {
        if (pin->wire == NULL) {
            icemu_pinlist_add(other->wire, pin);
            pin->wire = other->wire;
        } else if (other->wire == NULL) {
            icemu_pinlist_add(pin->wire, other);
            other->wire = pin->wire;
        } else {
            // TODO: merge the two wires
        }
    }
    wire_propagate(pin->wire);
}

void icemu_pinlist_init(PinList *pinlist, uint8_t capacity)
{
    pinlist->capacity = capacity;
    pinlist->count = 0;
    pinlist->pins = malloc(sizeof(Pin *) * capacity);
    memset(pinlist->pins, 0, sizeof(Pin *) * capacity);
}

void icemu_pinlist_destroy(PinList *pinlist)
{
    free(pinlist->pins);
    free(pinlist);
}

void icemu_pinlist_add(PinList *pinlist, Pin *pin)
{
    assert(pinlist->count < pinlist->capacity);

    pinlist->pins[pinlist->count] = pin;
    pinlist->count++;
}

int icemu_pinlist_find(PinList *pinlist, Pin *pin)
{
    uint8_t i;

    for (i = 0; i < pinlist->count; i++) {
        if (pinlist->pins[i] == pin) {
            return i;
        }
    }
    return -1;
}
