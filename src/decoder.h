#pragma once
#include "pin.h"

#define MAX_DECODER_INPUTS 4
#define MAX_DECODER_OUTPUTS 8

typedef struct {
    Pin * inputs[MAX_DECODER_INPUTS];
    Pin * outputs[MAX_DECODER_OUTPUTS];
} Decoder;

void icemu_SN74HC138_init(Chip *chip);

