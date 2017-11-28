#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "pin.h"
#include "pinlist.h"
#include "chip.h"
#include "util.h"

// This is used by the "Run until next trigger" feature in Simulation.
// If not null, it's called on *every* pin change.
static PinChangeFunc global_pinchange_trigger = NULL;

/* Private */
static void pin_trigger_change(ICePin *pin)
{
    if ((pin->chip != NULL) && (pin->chip->pin_change_func != NULL)) {
        pin->chip->pin_change_func(pin);
    }
    if (global_pinchange_trigger != NULL) {
        global_pinchange_trigger(pin);
    }
}

// We propagate to one or zero other pin on the wire. Our goal is to:
// 1. propagate state to all wired pins before any pin_change_func() call.
// 2. call pin_change_func() for each changed pin *after* propagation occured.

static void wire_propagate(ICePinList *wire)
{
    bool wire_is_high = false;
    unsigned int freq = 0;
    uint8_t i;
    ICePin *p;
    bool changed_pins[MAX_PINS_ON_A_WIRE] = { false };

    for (i = 0; i < wire->count; i++) {
        p = wire->pins[i];
        if (p->output && p->high) {
            wire_is_high = true;
            freq = MAX(freq, p->oscillating_freq);
        }
    }
    for (i = 0; i < wire->count; i++) {
        p = wire->pins[i];
        if (!p->output && ((p->high != wire_is_high) || (p->oscillating_freq != freq))) {
            p->high = wire_is_high;
            p->oscillating_freq = freq;
            changed_pins[i] = true;
        }
    }
    for (i = 0; i < wire->count; i++) {
        if (changed_pins[i]) {
            pin_trigger_change(wire->pins[i]);
        }
    }
}

/* We need to keep wired pins in a specific order to ensure proper execution order of pin_change()
   operations. This order is the same as the creation order on the chip. Therefore, if you want
   an input pin on a chip to be triggered before another input pin on the same chip when they're
   wired together, make sure that this pin is added to the chip first.
 */
static int wire_compare_pins(const void *a, const void *b)
{
    const ICePin *pa = *(const ICePin **)a;
    const ICePin *pb = *(const ICePin **)b;
    uint8_t indexa, indexb;

    if (pa->chip != NULL) {
        indexa = icemu_pinlist_find(&pa->chip->pins, pa);
    } else {
        indexa = 0;
    }
    if (pb->chip != NULL) {
        indexb = icemu_pinlist_find(&pb->chip->pins, pb);
    } else {
        indexb = 0;
    }
    return (int)indexa - (int)indexb;
}

/* Public */

ICePin* icemu_pin_new(Chip *chip, const char *code, bool output)
{
    ICePin *pin;
    pin = malloc(sizeof(ICePin));
    icemu_pin_init(pin, chip, code, output);
    return pin;
}

void icemu_pin_init(ICePin *pin, Chip *chip, const char *code, bool output)
{
    pin->chip = chip;
    pin->code = code;
    pin->output = output;
    pin->low_means_high = code[0] == '~';
    pin->high = false;
    pin->oscillating_freq = 0;
    pin->wire = NULL;
}

bool icemu_pin_set(ICePin *pin, bool high)
{
    if ((high != pin->high) || (pin->oscillating_freq > 0)) {
        pin->high = high;
        pin->oscillating_freq = 0;
        if (pin->output && (pin->wire != NULL)) {
            wire_propagate(pin->wire);
        }
        pin_trigger_change(pin);
        return true;
    }
    return false;
}

bool icemu_pin_enable(ICePin *pin, bool enabled)
{
    bool high;

    high = enabled != pin->low_means_high;
    return icemu_pin_set(pin, high);
}

bool icemu_pin_isenabled(const ICePin *pin)
{
    return pin->high != pin->low_means_high;
}

void icemu_pin_set_oscillating_freq(ICePin *pin, unsigned int freq)
{
    if (freq != pin->oscillating_freq) {
        pin->oscillating_freq = freq;
        if (freq > 0) {
            pin->high = true;
        }
        if (pin->output && (pin->wire != NULL)) {
            wire_propagate(pin->wire);
        }
        pin_trigger_change(pin);
    }
}

void icemu_pin_wireto(ICePin *pin, ICePin *other)
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
    // we need to sort to ensure proper propagation order. See comment near wire_compare_pins().
    qsort(pin->wire->pins, pin->wire->count, sizeof(ICePin *), wire_compare_pins);
    wire_propagate(pin->wire);
}

void icemu_pin_set_global_pinchange_trigger(PinChangeFunc func)
{
    global_pinchange_trigger = func;
}

