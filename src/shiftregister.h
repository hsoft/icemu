#pragma once
#include "pin.h"
#include "pinlist.h"

typedef struct {
    uint8_t buffer;
    Pin *clock;
    Pin *serial1;
    Pin *serial2;
    Pin *buffer_pin;
    Pin *enable_pin;
    Pin *reset_pin;
    ICePinList outputs;
} ShiftRegister;

void icemu_CD74AC164_init(Chip *chip);
void icemu_SN74HC595_init(Chip *chip);
