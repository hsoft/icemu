#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "sim.h"
#include "ui.h"
#include "util.h"

#define MAX_SIM_ACTIONS 30
#define MAX_SIM_CHIPS 256

typedef struct {
    char key;
    UIActionFunc func;
} UIAction;

typedef struct {
    bool running;
    time_t epoch; // usecs
    time_t ticks;
    time_t resolution; // usecs per tick
    RunloopFunc runloop;
    UIAction actions[MAX_SIM_ACTIONS];
    Chip * chips[MAX_SIM_CHIPS];
} Simulation;

static Simulation sim;

/* Private */
// the timestamp we need to be at to increase the tick
static time_t sim_clock_target()
{
    return sim.epoch + sim.ticks * sim.resolution;
}

static void sim_wait_until_next_tick()
{
    time_t ts, target;

    target = sim_clock_target();
    ts = icemu_util_timestamp();
    if (ts < target) {
        usleep(target - ts);
    }
}

static void sim_keypress(char key)
{
    int i;

    key = toupper(key);
    for (i = 0; i < MAX_SIM_ACTIONS; i++) {
        if (sim.actions[i].func == NULL) {
            break;
        }
        if (sim.actions[i].key == key) {
            sim.actions[i].func();
        }
    }
}

static void sim_tick()
{
    int key, i;

    sim.ticks++;
    for (i = 0; i < MAX_SIM_CHIPS; i++) {
        if (sim.chips[i] == NULL) {
            break;
        }
        icemu_chip_elapse(sim.chips[i], sim.resolution);
    }
    key = icemu_ui_refresh();
    if (key >= 0) {
        sim_keypress((char)key);
    }
}

/* Public */
void icemu_sim_init(time_t resolution, RunloopFunc runloop)
{
    sim.running = true;
    sim.epoch = icemu_util_timestamp();
    sim.ticks = 0;
    sim.resolution = resolution;
    sim.runloop = runloop;
    memset(sim.actions, 0, sizeof(UIAction) * MAX_SIM_ACTIONS);
    memset(sim.chips, 0, sizeof(Chip *) * MAX_SIM_CHIPS);
}

void icemu_sim_stop()
{
    sim.running = false;
}

void icemu_sim_add_chip(Chip *chip)
{
    int i;

    for (i = 0; i < MAX_SIM_CHIPS; i++) {
        if (sim.chips[i] == NULL) {
            sim.chips[i] = chip;
            break;
        }
    }
}

void icemu_sim_add_action(char key, char *label, UIActionFunc func)
{
    int i;

    for (i = 0; i < MAX_SIM_ACTIONS; i++) {
        if (sim.actions[i].func == NULL) {
            break;
        }
    }
    sim.actions[i].key = toupper(key);
    sim.actions[i].func = func;
    icemu_ui_add_label(label);
}

void icemu_sim_run()
{
    icemu_ui_init();
    icemu_sim_add_action('q', "(Q)uit", icemu_sim_stop);
    icemu_ui_refresh();
    while (sim.running) {
        sim_wait_until_next_tick();
        if (sim.runloop != NULL) {
            sim.runloop();
        }
        sim_tick();
    }
    icemu_ui_deinit();
}

void icemu_sim_delay(time_t usecs)
{
    time_t ts, target, tick_target;

    ts = icemu_util_timestamp();
    target = ts + usecs;
    tick_target = sim_clock_target();
    if (target > tick_target) {
        while (sim.running && (icemu_util_timestamp() < target)) {
            sim_wait_until_next_tick();
            sim_tick();
        }
    } else {
        usleep(usecs);
    }
}

time_t icemu_sim_elapsed_usecs()
{
    time_t ts;

    ts = icemu_util_timestamp();
    return ts - sim.epoch;
}
