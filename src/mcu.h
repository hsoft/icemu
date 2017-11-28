#pragma once
#include <sys/time.h>

#include "chip.h"
#include "pin.h"

#define MAX_INTERRUPTS 100
#define MAX_TIMERS 20

typedef void (ICeInterruptFunc)();

typedef enum {
    INTERRUPT_ON_RISING,
    INTERRUPT_ON_FALLING,
    INTERRUPT_ON_BOTH
} MCUInterruptType;

typedef struct {
    MCUInterruptType type;
    ICeInterruptFunc *func;
} MCUInterrupt;

typedef void (ICeTimerFunc)();

typedef struct {
    time_t every; // usecs
    time_t elapsed;
    ICeTimerFunc *func;
} MCUTimer;

typedef struct {
    // index of the interrupt func must be the same as the index of the pin the interrupt
    // is bound to.
    MCUInterrupt interrupts[MAX_INTERRUPTS];
    MCUTimer timers[MAX_TIMERS];
} MCU;

void icemu_mcu_add_interrupt(
    ICeChip *chip, ICePin *pin, MCUInterruptType type, ICeInterruptFunc *interrupt);
void icemu_mcu_add_timer(ICeChip *chip, time_t every_usecs, ICeTimerFunc *timer_func);

void icemu_ATtiny_init(ICeChip *chip);
