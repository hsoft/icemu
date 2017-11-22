#pragma once
#include "pin.h"

typedef struct {
    uint8_t buffer;
    bool isbuffered;
    Pin *clock;
    Pin *serial1;
    Pin *serial2;
    PinList outputs;
} ShiftRegister;

void icemu_CD74AC164_init(Chip *chip);
