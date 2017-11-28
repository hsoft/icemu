#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "pin.h"
#include "chip.h"
#include "util.h"

// This is used by the "Run until next trigger" feature in Simulation.
// If not null, it's called on *every* pin change.
static PinChangeFunc global_pinchange_trigger = NULL;

/* Private */
static void pin_trigger_change(Pin *pin)
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

static void wire_propagate(PinList *wire)
{
    bool wire_is_high = false;
    unsigned int freq = 0;
    uint8_t i;
    Pin *p;
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
    const Pin *pa = *(const Pin **)a;
    const Pin *pb = *(const Pin **)b;
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

Pin* icemu_pin_new(Chip *chip, const char *code, bool output)
{
    Pin *pin;
    pin = malloc(sizeof(Pin));
    icemu_pin_init(pin, chip, code, output);
    return pin;
}

void icemu_pin_init(Pin *pin, Chip *chip, const char *code, bool output)
{
    pin->chip = chip;
    pin->code = code;
    pin->output = output;
    pin->low_means_high = code[0] == '~';
    pin->high = false;
    pin->oscillating_freq = 0;
    pin->wire = NULL;
}

bool icemu_pin_set(Pin *pin, bool high)
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

bool icemu_pin_enable(Pin *pin, bool enabled)
{
    bool high;

    high = enabled != pin->low_means_high;
    return icemu_pin_set(pin, high);
}

bool icemu_pin_isenabled(Pin *pin)
{
    return pin->high != pin->low_means_high;
}

void icemu_pin_set_oscillating_freq(Pin *pin, unsigned int freq)
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
    // we need to sort to ensure proper propagation order. See comment near wire_compare_pins().
    qsort(pin->wire->pins, pin->wire->count, sizeof(Pin *), wire_compare_pins);
    wire_propagate(pin->wire);
}

void icemu_pin_set_global_pinchange_trigger(PinChangeFunc func)
{
    global_pinchange_trigger = func;
}

void icemu_pinlist_init(PinList *pinlist, uint8_t capacity)
{
    pinlist->capacity = capacity;
    pinlist->count = 0;
    pinlist->pins = malloc(sizeof(Pin *) * capacity);
    memset(pinlist->pins, 0, sizeof(Pin *) * capacity);
}

void icemu_pinlist_subset_of_existing(
    PinList *pinlist, PinList *existing, const char **codes)
{
    uint8_t count, i;

    count = icemu_util_chararray_count(codes);
    icemu_pinlist_init(pinlist, count);

    for (i = 0; i < count; i++) {
        icemu_pinlist_add(pinlist, icemu_pinlist_find_by_code(existing, codes[i]));
    }
}

void icemu_pinlist_destroy(PinList *pinlist)
{
    free(pinlist->pins);
    free(pinlist);
}

void icemu_pinlist_add(PinList *pinlist, Pin *pin)
{
    assert(pinlist->count < pinlist->capacity);
    assert(pin != NULL);

    pinlist->pins[pinlist->count] = pin;
    pinlist->count++;
}

int icemu_pinlist_find(const PinList *pinlist, const Pin *pin)
{
    uint8_t i;

    for (i = 0; i < pinlist->count; i++) {
        if (pinlist->pins[i] == pin) {
            return i;
        }
    }
    return -1;
}

Pin* icemu_pinlist_find_by_code(PinList *pinlist, const char *code)
{
    uint8_t i;
    const char *c;

    for (i = 0; i < pinlist->count; i++) {
        c = pinlist->pins[i]->code;
        if (c[0] == '~') {
            c++;
        }
        if (strncmp(code, c, MAX_CODE_LEN) == 0) {
            return pinlist->pins[i];
        }
    }
    return NULL;
}

bool icemu_pinlist_ishigh_any(PinList *pinlist)
{
    uint8_t i;
    for (i = 0; i < pinlist->count; i++) {
        if (pinlist->pins[i]->high) {
            return true;
        }
    }
    return false;
}

bool icemu_pinlist_ishigh_all(PinList *pinlist)
{
    uint8_t i;
    for (i = 0; i < pinlist->count; i++) {
        if (!pinlist->pins[i]->high) {
            return false;
        }
    }
    return true;
}

bool icemu_pinlist_isenabled_any(PinList *pinlist)
{
    uint8_t i;
    for (i = 0; i < pinlist->count; i++) {
        if (icemu_pin_isenabled(pinlist->pins[i])) {
            return true;
        }
    }
    return false;
}

bool icemu_pinlist_isenabled_all(PinList *pinlist)
{
    uint8_t i;
    for (i = 0; i < pinlist->count; i++) {
        if (!icemu_pin_isenabled(pinlist->pins[i])) {
            return false;
        }
    }
    return true;
}

void icemu_pinlist_set_all(PinList *pinlist, bool high)
{
    uint8_t i;
    for (i = 0; i < pinlist->count; i++) {
        icemu_pin_set(pinlist->pins[i], high);
    }
}

void icemu_pinlist_enable_all(PinList *pinlist, bool enabled)
{
    uint8_t i;
    for (i = 0; i < pinlist->count; i++) {
        icemu_pin_enable(pinlist->pins[i], enabled);
    }
}

unsigned int icemu_pinlist_oscillating_freq(PinList *pinlist)
{
    unsigned int i, result = 0;

    for (i = 0; i < pinlist->count; i++) {
        result = MAX(pinlist->pins[i]->oscillating_freq, result);
    }
    return result;
}
