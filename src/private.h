#pragma once

#include "pin.h"

void icemu_pin_set_global_pinchange_trigger(ICePinChangeFunc *func);

void icemu_chip_init(
    ICeChip *chip, void *logical_unit, ICePinChangeFunc *pin_change_func, uint8_t pin_count);
ICePin* icemu_chip_addpin(ICeChip *chip, const char *code, bool output);
void icemu_chip_addpins(ICeChip *chip, ICePinList *dst_pinlist, const char **codes, bool output);
void icemu_chip_elapse(ICeChip *chip, time_t usecs);
