#pragma once
#include <stdbool.h>

#include "chip.h"

typedef struct {
    uint32_t fade_timeout;
    bool powered;
    Pin *vcc;
    Pin *gnd;
} LED;

typedef struct {
    uint8_t width;
    uint8_t height;
    Pin vcc;
    LED *leds;
} LEDMatrix;

void icemu_seg7_init(Chip *chip);
void icemu_ledmatrix_init(Chip *chip, uint8_t width, uint8_t height);
Pin* icemu_ledmatrix_vcc(Chip *chip);
