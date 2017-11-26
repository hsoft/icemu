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

typedef void (*UIActionFunc)();

typedef void (*RunloopFunc)();

void icemu_sim_init(time_t resolution, RunloopFunc runloop);
void icemu_sim_stop();
void icemu_sim_add_chip(Chip *chip);
void icemu_sim_add_action(char key, char *label, UIActionFunc func);
void icemu_sim_run();
void icemu_sim_delay(time_t usecs);
time_t icemu_sim_elapsed_usecs();
