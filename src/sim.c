#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "sim.h"
#include "ui.h"
#include "util.h"
#include "private.h"

static Simulation sim;

/* Private */
static void sim_wait_until_next_tick()
{
    time_t ts, target;

    target = sim.last_tick_ts + sim.elapsing_for;
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
    int i;
    unsigned int elapse;
    unsigned int elapse_min = ICE_MAX_CHIP_ELAPSE;
    ICeChip **chip_registry = icemu_chip_get_registry();

    // We've *already* elapsed our `elapsing_for` usecs. While we're calling our elapse() funcs,
    // the clock is still ticking! this is why we take our timestamp now.

    sim.last_tick_ts = icemu_util_timestamp();
    sim.elapsed += sim.elapsing_for;
    for (i = 0; i < MAX_SIM_CHIPS; i++) {
        if (chip_registry[i] == NULL) {
            break;
        }
        sim.elapsing_chip = chip_registry[i];
        elapse = icemu_chip_elapse(chip_registry[i], sim.elapsing_for);
        sim.elapsing_chip = NULL;
        if (elapse > 0) {
            elapse_min = MIN(elapse_min, elapse);
        }
    }
    sim.elapsing_for = elapse_min;
}

// Returns the number of usecs elapsed in the simulation (if paused, returns 0)
static time_t sim_loop_once()
{
    int key;
    time_t result;

    if (sim.runmode != ICE_SIM_RUNMODE_PAUSE) {
        result = sim.elapsing_for;
        sim_wait_until_next_tick();
        sim_tick();
    } else {
        result = 0;
    }
    key = icemu_ui_refresh();
    if (key >= 0) {
        sim_keypress((char)key);
    }
    return result;
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

/* Protected */
Simulation* icemu_sim_get()
{
    return &sim;
}

/* Public */
void icemu_sim_init()
{
    sim.initialized = true;
    sim.runmode = ICE_SIM_RUNMODE_RUN;
    sim.slowdown_factor = 1;
    sim.last_tick_ts = icemu_util_timestamp();
    sim.elapsed = 0;
    sim.elapsing_for = 1;
    sim.elapsing_chip = NULL;
    memset(sim.actions, 0, sizeof(sim.actions));
    memset(sim.triggers, 0, sizeof(sim.triggers));
    memset(sim.debug_values, 0, sizeof(sim.debug_values));
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
    icemu_sim_add_action('t', "Run one s(t)ep", sim_run_one_tick);
    if (sim.triggers[0] != NULL) {
        icemu_sim_add_action('g', "Run until tri(g)ger", sim_run_until_trigger);
        icemu_pin_set_global_pinchange_trigger(sim_global_pinchange);
    }
    icemu_ui_refresh();
    while (sim.runmode != ICE_SIM_RUNMODE_STOP) {
        sim_loop_once();
    }
    icemu_ui_deinit();
}

void icemu_sim_delay(time_t usecs)
{
    while (usecs >= sim.elapsing_for) {
        usecs -= sim_loop_once();
        if (sim.runmode == ICE_SIM_RUNMODE_STOP) {
            return;
        }
    }
    usleep(usecs);
}

ICeChip* icemu_sim_get_elapsing_chip()
{
    return sim.elapsing_chip;
}

ICeSimRunMode icemu_sim_runmode()
{
    return sim.runmode;
}

void icemu_sim_set_runmode(ICeSimRunMode runmode)
{
    sim.runmode = runmode;
}

unsigned int icemu_sim_slowdown_factor()
{
    return sim.slowdown_factor;
}

void icemu_sim_set_slowdown_factor(unsigned int slowdown_factor)
{
    sim.slowdown_factor = slowdown_factor;
}

void icemu_sim_set_debug_value(const char *name, int val)
{
    int i;
    struct DebugValue *dv;

    for (i = 0; i < MAX_DEBUG_VALUES; i++) {
        dv = &sim.debug_values[i];
        if (dv->name == NULL) {
            dv->name = name;
            dv->val = val;
            break;
        } else if (strcmp(dv->name, name) == 0) {
            dv->val = val;
            break;
        }
    }
}
