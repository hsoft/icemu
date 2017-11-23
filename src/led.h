#pragma once
#include <stdbool.h>

#include "chip.h"

typedef struct {
    uint32_t fade_timeout;
    bool powered;
    bool anode; // if true, is lit when the pin is LOW
} LED;

typedef struct {
    uint8_t width;
    uint8_t height;
    LED *leds;
} LEDMatrix;

void icemu_seg7_init(Chip *chip);
void icemu_ledmatrix_init(Chip *chip, uint8_t width, uint8_t height);
