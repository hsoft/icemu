#pragma once

#include "pin.h"
#include "sim.h"

#define MAX_SIM_ACTIONS 30
#define MAX_SIM_TRIGGERS 100
#define MAX_DEBUG_VALUES 30
#define MAX_SIM_CHIPS 256

struct DebugValue {
    const char *name;
    int val;
};

struct ChipStack {
    ICeChip *chip;
    struct ChipStack *prev;
};

typedef struct {
    bool initialized;
    ICeSimRunMode runmode;
    time_t last_tick_ts; // usecs
    unsigned int elapsed;
    unsigned int elapsing_for;
    unsigned int slowdown_factor;
    ICeUIAction actions[MAX_SIM_ACTIONS];
    ICePin * triggers[MAX_SIM_TRIGGERS];
    struct DebugValue debug_values[MAX_DEBUG_VALUES];
    struct ChipStack *elapsing_chip_stack;
} Simulation;

void icemu_pin_set_global_pinchange_trigger(ICePinChangeFunc *func);

void icemu_chip_init(
    ICeChip *chip, void *logical_unit, ICePinChangeFunc *pin_change_func, uint8_t pin_count);
ICePin* icemu_chip_addpin(ICeChip *chip, const char *code, bool output);
void icemu_chip_addpins(ICeChip *chip, ICePinList *dst_pinlist, const char **codes, bool output);
unsigned int icemu_chip_elapse(ICeChip *chip, time_t usecs);
ICeChip** icemu_chip_get_registry();

Simulation* icemu_sim_get();

