#pragma once
#include "pin.h"

#define MAX_GATE_COUNT 10

typedef bool (*GateTestFunc)(PinList *);

typedef struct {
    GateTestFunc test_func;
    PinList inputs;
    Pin *output;
} Gate;

typedef struct {
    Gate * gates[MAX_GATE_COUNT];
} GateSet;

void icemu_CD4001B_init(Chip *chip);

