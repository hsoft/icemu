#pragma once
#include "pin.h"
#include "pinlist.h"

typedef struct {
    uint8_t buffer;
    ICePin *clock;
    ICePin *serial1;
    ICePin *serial2;
    ICePin *buffer_pin;
    ICePin *enable_pin;
    ICePin *reset_pin;
    ICePinList outputs;
} ShiftRegister;

void icemu_CD74AC164_init(Chip *chip);
void icemu_SN74HC595_init(Chip *chip);
