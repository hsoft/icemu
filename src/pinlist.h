#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "pin.h"

struct ICePinList {
    uint8_t capacity;
    uint8_t count;
    Pin **pins;
};

ICePinList* icemu_pinlist_new(uint8_t capacity);
void icemu_pinlist_init(ICePinList *ICePinList, uint8_t capacity);
void icemu_pinlist_subset_of_existing(
    ICePinList *pinlist, const ICePinList *existing, const char **codes);
void icemu_pinlist_destroy(ICePinList *pinlist);
void icemu_pinlist_add(ICePinList *pinlist, Pin *pin);
int icemu_pinlist_find(const ICePinList *pinlist, const Pin *pin);
Pin* icemu_pinlist_find_by_code(const ICePinList *pinlist, const char *code);
bool icemu_pinlist_ishigh_any(const ICePinList *pinlist);
bool icemu_pinlist_ishigh_all(const ICePinList *pinlist);
bool icemu_pinlist_isenabled_any(const ICePinList *pinlist);
bool icemu_pinlist_isenabled_all(const ICePinList *pinlist);
void icemu_pinlist_set_all(ICePinList *pinlist, bool high);
void icemu_pinlist_enable_all(ICePinList *pinlist, bool enabled);
unsigned int icemu_pinlist_oscillating_freq(const ICePinList *pinlist);
