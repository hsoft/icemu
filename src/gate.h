#pragma once
#include "pin.h"
#include "pinlist.h"

#define MAX_GATE_COUNT 10

typedef enum {
    ICE_TEST_RESULT_HIGH = 0,
    ICE_TEST_RESULT_LOW = 1,
    ICE_TEST_RESULT_OSCILLATE = 2
} ICeGateTestResult;

typedef ICeGateTestResult (ICeGateTestFunc)(ICePinList *);

typedef struct {
    ICeGateTestFunc *test_func;
    ICePinList inputs;
    ICePin *output;
} Gate;

typedef struct {
    Gate * gates[MAX_GATE_COUNT];
} GateSet;

/* NOR */
void icemu_CD4001B_init(ICeChip *chip);
void icemu_CD4002B_init(ICeChip *chip);
void icemu_CD4025B_init(ICeChip *chip);
void icemu_SN74LS02_init(ICeChip *chip);
void icemu_SN74LS27_init(ICeChip *chip);
void icemu_SN54ALS27A_init(ICeChip *chip);
void icemu_SN54AAS27_init(ICeChip *chip);
void icemu_SN5427_init(ICeChip *chip);
void icemu_SN7427_init(ICeChip *chip);
void icemu_SN54LS27_init(ICeChip *chip);
void icemu_SN74ALS27A_init(ICeChip *chip);
void icemu_SN74AS27_init(ICeChip *chip);
void icemu_SN54S260_init(ICeChip *chip);
void icemu_SN74S260_init(ICeChip *chip);
void icemu_SN74F260_init(ICeChip *chip);

/* Inverter */
void icemu_SN74HC14_init(ICeChip *chip);

