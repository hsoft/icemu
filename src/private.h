#pragma once

#include "pin.h"
#include "sim.h"

#define MAX_SIM_ACTIONS 30
#define MAX_SIM_CHIPS 256
#define MAX_SIM_TRIGGERS 100
#define MAX_DEBUG_VALUES 30

struct DebugValue {
    const char *name;
    int val;
};

typedef struct {
    bool initialized;
    ICeSimRunMode runmode;
    time_t next_tick_target; // usecs
    time_t ticks;
    time_t resolution; // usecs per tick
    unsigned int slowdown_factor;
    ICeUIAction actions[MAX_SIM_ACTIONS];
    ICeChip * chips[MAX_SIM_CHIPS];
    ICePin * triggers[MAX_SIM_TRIGGERS];
    struct DebugValue debug_values[MAX_DEBUG_VALUES];
} Simulation;

void icemu_pin_set_global_pinchange_trigger(ICePinChangeFunc *func);

void icemu_chip_init(
    ICeChip *chip, void *logical_unit, ICePinChangeFunc *pin_change_func, uint8_t pin_count);
ICePin* icemu_chip_addpin(ICeChip *chip, const char *code, bool output);
void icemu_chip_addpins(ICeChip *chip, ICePinList *dst_pinlist, const char **codes, bool output);
void icemu_chip_elapse(ICeChip *chip, time_t usecs);

Simulation* icemu_sim_get();
void icemu_sim_add_chip(ICeChip *chip);

