#pragma once
#include <sys/time.h>

#include "chip.h"
#include "pin.h"

#define ICE_MAX_INTERRUPTS 100
#define ICE_MAX_TIMERS 20

typedef void (ICeInterruptFunc)();

typedef enum {
    ICE_INTERRUPT_ON_RISING,
    ICE_INTERRUPT_ON_FALLING,
    ICE_INTERRUPT_ON_BOTH
} ICeMCUInterruptType;

typedef struct {
    ICeMCUInterruptType type;
    ICeInterruptFunc *func;
} ICeMCUInterrupt;

typedef void (ICeTimerFunc)();

typedef struct {
    time_t every; // usecs
    time_t elapsed;
    ICeTimerFunc *func;
} ICeMCUTimer;

typedef struct {
    // index of the interrupt func must be the same as the index of the pin the interrupt
    // is bound to.
    ICeMCUInterrupt interrupts[ICE_MAX_INTERRUPTS];
    ICeMCUTimer timers[ICE_MAX_TIMERS];
} ICeMCU;

void icemu_mcu_add_interrupt(
    ICeChip *chip, ICePin *pin, ICeMCUInterruptType type, ICeInterruptFunc *interrupt);
void icemu_mcu_add_timer(ICeChip *chip, time_t every_usecs, ICeTimerFunc *timer_func);

void icemu_ATtiny_init(ICeChip *chip);
