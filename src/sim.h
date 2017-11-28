#pragma once
#include <sys/time.h>

#include "chip.h"

/* The goal of this unit is to keep track of time in a unified manner on a set of chips
   (a circuit). It can also control speed of the simulation.

   The simulation is centered around the concept of "tick", that is, the number of usecs
   that is required to go through the runloop once. When you create the simulation, you
   should feed it with a value that approaches the time it really takes for your MCU to
   execute the runloop.

   Each time the simulation runs the loop, it waits until our target clock is reached
   before running the loop again. That makes time control very easy: to slow down the
   simulation, just wait a bit longer.

   This unit doesn't only make the runloop run slower. It also takes care of sending the
   proper amount of "elapsed usecs" to each chip under its control. Not every chip is
   time aware, but the important ones are (MCU, LED).

   The simulation also takes care of keeping the UI refreshed. One tricky situation it
   deals with is delay() calls that last longer than a runloop. If the code wants to
   wait 1s, you don't want to stop UI refreshes for this.
 */

typedef void (ICeUIActionFunc)();

typedef void (ICeRunloopFunc)();

typedef struct {
    char key;
    ICeUIActionFunc *func;
} ICeUIAction;

typedef enum {
    ICE_SIM_RUNMODE_RUN,
    ICE_SIM_RUNMODE_PAUSE,
    ICE_SIM_RUNMODE_TRIGGER,
    ICE_SIM_RUNMODE_STOP
} ICeSimRunMode;

/* Initialize the simulation
 *
 * resolution: the amount of usecs that each runloop pass is going to take. If it's executed
 *             faster, we wait before running the next pass. You should set this value to the
 *             expected time (including delay() calls) *on the MCU* for a proper simulation.
 * runloop: a pointer to your runloop. The runloop is a function that executes *one pass*, not
 *          an infinite loop. This is going to be called once every tick.
 */
void icemu_sim_init(time_t resolution, ICeRunloopFunc *runloop);

/* Adds a chip to the simulation
 *
 * When doing so, that chips "elapse" function will be called appropriately. Not all chips do
 * stuff on "elapse()", but it doesn't hurt to add all your chips. You will always want to add
 * your MCU, otherwise timers won't work.
 */
void icemu_sim_add_chip(ICeChip *chip);

/* Add custom action to the keybindings
 */
void icemu_sim_add_action(char key, char *label, ICeUIActionFunc *func);

/* Add global pin trigger
 *
 * This is for the "Run until next trigger" feature. Adding a pin with this function will make
 * the simulation stop on that pin's next change when we're in "Run until next trigger" mode.
 */
void icemu_sim_add_trigger(ICePin *pin);

/* Run the simulation
 *
 * This is the main function of the simulation. Run this after you're finished configuring the
 * thing. This function returns when the user decided to quit de simulation.
 */
void icemu_sim_run();

/* Sleep of `usecs`
 *
 * This is equivalent to calling `usleep(usecs)`, with one caveat: if `usecs` is greater than
 * the simulation resolution, this function properly refreshes the UI (without calling the
 * runloop). Use this function as a shim for "delay()" calls on your MCU.
 */
void icemu_sim_delay(time_t usecs);

/* Simulation Properties */
time_t icemu_sim_elapsed_usecs();
ICeSimRunMode icemu_sim_runmode();
void icemu_sim_set_runmode(ICeSimRunMode runmode);
time_t icemu_sim_resolution();
time_t icemu_sim_ticks();
