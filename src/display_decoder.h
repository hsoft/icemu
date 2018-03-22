#pragma once
#include "pin.h"
#include "pinlist.h"
#include "chip.h"

typedef struct {
    ICePinList inputs;
    ICePinList outputs;
    ICePinList serial_pins;
    uint8_t *value_table;
} ICeDisplayDecoder;

void icemu_displaydecoder_wireto_seg7(ICeChip *chip, ICeChip *seg7);
void icemu_SN7447A_init(ICeChip *chip);


