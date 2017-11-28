#pragma once
#include <stdbool.h>

#include "chip.h"

typedef struct {
    uint32_t fade_timeout;
    bool powered;
    ICePin *vcc;
    ICePin *gnd;
} LED;

typedef struct {
    uint8_t width;
    uint8_t height;
    ICePin vcc;
    LED *leds;
} LEDMatrix;

void icemu_seg7_init(ICeChip *chip);
void icemu_ledmatrix_init(ICeChip *chip, uint8_t width, uint8_t height);
ICePin* icemu_ledmatrix_vcc(ICeChip *chip);
