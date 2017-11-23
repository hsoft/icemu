#pragma once
#include <stdbool.h>

#include "chip.h"

typedef struct {
    uint8_t width;
    uint8_t height;
} LEDMatrix;

void icemu_seg7_init(Chip *chip);
void icemu_ledmatrix_init(Chip *chip, uint8_t width, uint8_t height);
