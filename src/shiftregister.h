#pragma once
#include "pin.h"

#define MAX_SHIFTREGISTER_OUTPUTS 8

typedef struct {
    uint8_t buffer;
    bool isbuffered;
    Pin *clock;
    Pin *serial1;
    Pin *serial2;
    Pin * outputs[MAX_SHIFTREGISTER_OUTPUTS];
} ShiftRegister;

uint8_t icemu_shiftregister_outputcount(ShiftRegister *sr);
void icemu_CD74AC164_init(Chip *chip);
