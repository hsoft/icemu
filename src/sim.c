#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "sim.h"
#include "ui.h"
#include "util.h"

#define MAX_SIM_ACTIONS 30
#define MAX_SIM_CHIPS 256
#define MAX_SIM_TRIGGERS 100

typedef struct {
    ICeSimRunMode runmode;
    time_t next_tick_target; // usecs
    time_t ticks;
    time_t resolution; // usecs per tick
    ICeRunloopFunc *runloop;
    ICeUIAction actions[MAX_SIM_ACTIONS];
    ICeChip * chips[MAX_SIM_CHIPS];
    ICePin * triggers[MAX_SIM_TRIGGERS];
} Simulation;

static Simulation sim;

/* Private */
static void sim_wait_until_next_tick()
{
    time_t ts;

    ts = icemu_util_timestamp();
    if (ts < sim.next_tick_target) {
        usleep(sim.next_tick_target - ts);
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
    int i;

    sim.ticks++;
    for (i = 0; i < MAX_SIM_CHIPS; i++) {
        if (sim.chips[i] == NULL) {
            break;
        }
        icemu_chip_elapse(sim.chips[i], sim.resolution);
    }
    sim.next_tick_target = icemu_util_timestamp() + sim.resolution;
}

static bool sim_loop_once(ICeRunloopFunc *runloop)
{
    int key;

    if (sim.runmode != ICE_SIM_RUNMODE_PAUSE) {
        sim_wait_until_next_tick();
        if (runloop != NULL) {
            runloop();
        }
        sim_tick();
    }
    key = icemu_ui_refresh();
    if (key >= 0) {
        sim_keypress((char)key);
    }
    return sim.runmode != ICE_SIM_RUNMODE_PAUSE;
}

static void sim_stop()
{
    sim.runmode = ICE_SIM_RUNMODE_STOP;
}

static void sim_toggle_paused()
{
    if (sim.runmode == ICE_SIM_RUNMODE_PAUSE) {
        sim.runmode = ICE_SIM_RUNMODE_RUN;
    } else {
        sim.runmode = ICE_SIM_RUNMODE_PAUSE;
    }
}

static void sim_run_one_tick()
{
    sim.runmode = ICE_SIM_RUNMODE_PAUSE;
    sim_tick();
}

static void sim_run_until_trigger()
{
    sim.runmode = ICE_SIM_RUNMODE_TRIGGER;
}

static void sim_global_pinchange(ICePin *pin)
{
    ICePin **p;

    if (sim.runmode == ICE_SIM_RUNMODE_TRIGGER) {
        p = sim.triggers;
        while (*p != NULL) {
            if (*p == pin) {
                sim.runmode = ICE_SIM_RUNMODE_PAUSE;
            }
            p++;
        }
    }
}

/* Public */
void icemu_sim_init(time_t resolution, ICeRunloopFunc *runloop)
{
    sim.runmode = ICE_SIM_RUNMODE_RUN;
    sim.ticks = 0;
    sim.resolution = resolution;
    sim.next_tick_target = icemu_util_timestamp() + sim.resolution;
    sim.runloop = runloop;
    memset(sim.actions, 0, sizeof(sim.actions));
    memset(sim.chips, 0, sizeof(sim.chips));
    memset(sim.triggers, 0, sizeof(sim.triggers));
}

Simulation* icemu_sim_get()
{
    return &sim;
}

void icemu_sim_add_chip(ICeChip *chip)
{
    int i;

    for (i = 0; i < MAX_SIM_CHIPS; i++) {
        if (sim.chips[i] == NULL) {
            sim.chips[i] = chip;
            break;
        }
    }
}

void icemu_sim_add_action(char key, char *label, ICeUIActionFunc *func)
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

void icemu_sim_add_trigger(ICePin *pin)
{
    ICePin **p = sim.triggers;

    while (*p != NULL) p++;
    *p = pin;
}

void icemu_sim_run()
{
    icemu_ui_init();
    icemu_sim_add_action('q', "(Q)uit", sim_stop);
    icemu_sim_add_action('p', "(P)ause/Resume", sim_toggle_paused);
    icemu_sim_add_action('t', "Run one (t)ick", sim_run_one_tick);
    if (sim.triggers[0] != NULL) {
        icemu_sim_add_action('g', "Run until tri(g)ger", sim_run_until_trigger);
        icemu_pin_set_global_pinchange_trigger(sim_global_pinchange);
    }
    icemu_ui_refresh();
    while (sim.runmode != ICE_SIM_RUNMODE_STOP) {
        sim_loop_once(sim.runloop);
    }
    icemu_ui_deinit();
}

void icemu_sim_delay(time_t usecs)
{
    while (usecs >= sim.resolution) {
        if (sim_loop_once(NULL)) {
            usecs -= sim.resolution;
        }
        if (sim.runmode == ICE_SIM_RUNMODE_STOP) {
            return;
        }
    }
    usleep(usecs);
}

time_t icemu_sim_elapsed_usecs()
{
    return sim.ticks * sim.resolution;
}

ICeSimRunMode icemu_sim_runmode()
{
    return sim.runmode;
}

void icemu_sim_set_runmode(ICeSimRunMode runmode)
{
    sim.runmode = runmode;
}

time_t icemu_sim_resolution()
{
    return sim.resolution;
}

time_t icemu_sim_ticks()
{
    return sim.ticks;
}
