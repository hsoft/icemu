#pragma once
#include <sys/time.h>

#include "chip.h"
#include "pin.h"

#define ICE_MAX_INTERRUPTS 100
#define ICE_MAX_TIMERS 20
#define ICE_MCU_DEFAULT_RESOLUTION 50

typedef void (ICeRunloopFunc)();

typedef void (ICeInterruptFunc)();

typedef enum {
    ICE_INTERRUPT_ON_RISING,
    ICE_INTERRUPT_ON_FALLING,
    ICE_INTERRUPT_ON_BOTH
} ICeMCUInterruptType;

typedef struct {
    ICeMCUInterruptType type;
    ICePin *pin;
    ICeInterruptFunc *func;
    // number of usecs elapsed since last interrupt. Used for oscillating pins.
    unsigned int usecs_since_last;
} ICeMCUInterrupt;

typedef void (ICeTimerFunc)();

typedef struct {
    time_t every; // usecs
    time_t elapsed;
    ICeTimerFunc *func;
} ICeMCUTimer;

typedef struct {
    ICeRunloopFunc *runloop;
    bool in_runloop;
    time_t resolution; // usecs per runloop
    time_t usecs_since_last_run;
    // index of the interrupt func must be the same as the index of the pin the interrupt
    // is bound to.
    ICeMCUInterrupt interrupts[ICE_MAX_INTERRUPTS];
    ICeMCUTimer timers[ICE_MAX_TIMERS];
} ICeMCU;

/* Sets the runloop for the MCU
 *
 * The runloop is a function that executes *one pass*, not an infinite loop. This is going to be
 * called once every tick.
 */
void icemu_mcu_set_runloop(ICeChip *chip, ICeRunloopFunc *runloop);
void icemu_mcu_add_interrupt(
    ICeChip *chip, ICePin *pin, ICeMCUInterruptType type, ICeInterruptFunc *interrupt);
void icemu_mcu_add_timer(ICeChip *chip, time_t every_usecs, ICeTimerFunc *timer_func);

void icemu_ATtiny_init(ICeChip *chip);
