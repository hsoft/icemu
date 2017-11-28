#pragma once
#include <stdbool.h>

#include "chip.h"

void icemu_seg7_init(ICeChip *chip);
void icemu_ledmatrix_init(ICeChip *chip, uint8_t width, uint8_t height);
ICePin* icemu_ledmatrix_vcc(ICeChip *chip);
