#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "pinlist.h"
#include "util.h"

/* Public */
ICePinList* icemu_pinlist_new(uint8_t capacity)
{
    ICePinList *result;
    result = malloc(sizeof(ICePinList));
    icemu_pinlist_init(result, capacity);
    return result;
}

void icemu_pinlist_init(ICePinList *pinlist, uint8_t capacity)
{
    pinlist->capacity = capacity;
    pinlist->count = 0;
    pinlist->pins = malloc(sizeof(ICePin *) * capacity);
    memset(pinlist->pins, 0, sizeof(ICePin *) * capacity);
}

void icemu_pinlist_subset_of_existing(
    ICePinList *pinlist, const ICePinList *existing, const char **codes)
{
    uint8_t count, i;

    count = icemu_util_chararray_count(codes);
    icemu_pinlist_init(pinlist, count);

    for (i = 0; i < count; i++) {
        icemu_pinlist_add(pinlist, icemu_pinlist_find_by_code(existing, codes[i]));
    }
}

void icemu_pinlist_destroy(ICePinList *pinlist)
{
    free(pinlist->pins);
    free(pinlist);
}

void icemu_pinlist_add(ICePinList *pinlist, ICePin *pin)
{
    assert(pinlist->count < pinlist->capacity);
    assert(pin != NULL);

    pinlist->pins[pinlist->count] = pin;
    pinlist->count++;
}

void icemu_pinlist_add_multiple(ICePinList *pinlist, const ICePinList *other)
{
    int i;

    for (i = 0; i < other->count; i++) {
        icemu_pinlist_add(pinlist, other->pins[i]);
    }
}

int icemu_pinlist_find(const ICePinList *pinlist, const ICePin *pin)
{
    uint8_t i;

    for (i = 0; i < pinlist->count; i++) {
        if (pinlist->pins[i] == pin) {
            return i;
        }
    }
    return -1;
}

ICePin* icemu_pinlist_find_by_code(const ICePinList *pinlist, const char *code)
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

bool icemu_pinlist_ishigh_any(const ICePinList *pinlist)
{
    uint8_t i;
    for (i = 0; i < pinlist->count; i++) {
        if (pinlist->pins[i]->high) {
            return true;
        }
    }
    return false;
}

bool icemu_pinlist_ishigh_all(const ICePinList *pinlist)
{
    uint8_t i;
    for (i = 0; i < pinlist->count; i++) {
        if (!pinlist->pins[i]->high) {
            return false;
        }
    }
    return true;
}

bool icemu_pinlist_isenabled_any(const ICePinList *pinlist)
{
    uint8_t i;
    for (i = 0; i < pinlist->count; i++) {
        if (icemu_pin_isenabled(pinlist->pins[i])) {
            return true;
        }
    }
    return false;
}

bool icemu_pinlist_isenabled_all(const ICePinList *pinlist)
{
    uint8_t i;
    for (i = 0; i < pinlist->count; i++) {
        if (!icemu_pin_isenabled(pinlist->pins[i])) {
            return false;
        }
    }
    return true;
}

void icemu_pinlist_set_all(ICePinList *pinlist, bool high)
{
    uint8_t i;
    for (i = 0; i < pinlist->count; i++) {
        icemu_pin_set(pinlist->pins[i], high);
    }
}

void icemu_pinlist_enable_all(ICePinList *pinlist, bool enabled)
{
    uint8_t i;
    for (i = 0; i < pinlist->count; i++) {
        icemu_pin_enable(pinlist->pins[i], enabled);
    }
}

unsigned int icemu_pinlist_oscillating_freq(const ICePinList *pinlist)
{
    unsigned int i, result = 0;

    for (i = 0; i < pinlist->count; i++) {
        result = MAX(pinlist->pins[i]->oscillating_freq, result);
    }
    return result;
}
