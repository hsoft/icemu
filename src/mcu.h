#pragma once
#include <sys/time.h>

#include "chip.h"
#include "pin.h"

// this is the number of usecs that elapses at each runloop pass. If our simulation does a pass
// faster than this, we wait a little bit before continuing.
#define MCU_TIME_RESOLUTION 50

#define MAX_INTERRUPTS 100
#define MAX_TIMERS 20

typedef void (*InterruptFunc)();
typedef void (*TimerFunc)();

typedef struct {
    time_t every; // usecs
    time_t elapsed;
    TimerFunc func;
} MCUTimer;

typedef struct {
    time_t epoch; // usecs
    time_t ticks;
    // index of the interrupt func must be the same as the index of the pin the interrupt
    // is bound to.
    InterruptFunc interrupts[MAX_INTERRUPTS];
    MCUTimer timers[MAX_TIMERS];
} MCU;

void icemu_mcu_add_interrupt(Chip *chip, Pin *pin, InterruptFunc interrupt);
void icemu_mcu_add_timer(Chip *chip, time_t every_usecs, TimerFunc timer_func);
void icemu_mcu_tick(Chip *chip);

void icemu_ATtiny_init(Chip *chip);
