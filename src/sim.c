#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "sim.h"
#include "ui.h"
#include "util.h"

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

static void sim_loop_once(RunloopFunc runloop)
{
    int key;

    if (!sim.paused) {
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
}

static void sim_toggle_paused()
{
    sim.paused = !sim.paused;
}

static void sim_run_one_tick()
{
    sim.paused = true;
    sim_tick();
}

/* Public */
void icemu_sim_init(time_t resolution, RunloopFunc runloop)
{
    sim.running = true;
    sim.ticks = 0;
    sim.resolution = resolution;
    sim.next_tick_target = icemu_util_timestamp() + sim.resolution;
    sim.paused = false;
    sim.runloop = runloop;
    memset(sim.actions, 0, sizeof(UIAction) * MAX_SIM_ACTIONS);
    memset(sim.chips, 0, sizeof(Chip *) * MAX_SIM_CHIPS);
}

Simulation* icemu_sim_get()
{
    return &sim;
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
    icemu_sim_add_action('p', "(P)ause/Resume", sim_toggle_paused);
    icemu_sim_add_action('t', "Run one (t)ick", sim_run_one_tick);
    icemu_ui_refresh();
    while (sim.running) {
        sim_loop_once(sim.runloop);
    }
    icemu_ui_deinit();
}

void icemu_sim_delay(time_t usecs)
{
    while (usecs >= sim.resolution) {
        sim_loop_once(NULL);
        usecs -= sim.resolution;
    }
    usleep(usecs);
}

time_t icemu_sim_elapsed_usecs()
{
    return sim.ticks * sim.resolution;
}

