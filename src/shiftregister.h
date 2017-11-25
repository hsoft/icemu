#pragma once
#include "pin.h"

typedef struct {
    uint8_t buffer;
    Pin *clock;
    Pin *serial1;
    Pin *serial2;
    Pin *buffer_pin;
    Pin *enable_pin;
    PinList outputs;
} ShiftRegister;

void icemu_CD74AC164_init(Chip *chip);
void icemu_SN74HC595_init(Chip *chip);
