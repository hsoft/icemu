#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "mcu.h"
#include "util.h"
#include "private.h"

/* Private */
static void mcu_interrupt_elapse(ICeMCUInterrupt *interrupt, time_t usecs)
{
    unsigned int freq, interrupt_count;

    freq = interrupt->pin->oscillating_freq;
    if (freq > 0) {
        if (interrupt->type == ICE_INTERRUPT_ON_BOTH) {
            freq *= 2;
        }
        interrupt_count = (freq * usecs) / (1000UL * 1000UL);
        if (interrupt_count > 0) {
            // easy: just interrupt "count" times.
            interrupt->usecs_since_last = 0;
            while (interrupt_count > 0) {
                interrupt->func();
                interrupt_count--;
            }
        } else {
            // we don't interrupt on each tick. use usecs_since_last to know when to interrupt.
            interrupt->usecs_since_last += usecs;
            if ((1000UL * 1000UL) / freq < interrupt->usecs_since_last) {
                interrupt->usecs_since_last = 0;
                interrupt->func();
            }
        }
    } else {
        interrupt->usecs_since_last += usecs;
    }
}

static void mcu_timer_elapse(ICeMCUTimer *timer, time_t usecs)
{
    timer->elapsed += usecs;
    if (timer->elapsed >= timer->every) {
        timer->elapsed -= timer->every;
        timer->func();
    }
}

static void mcu_pinchange(ICePin *pin)
{
    int pinindex;
    ICeMCU *mcu;
    ICeMCUInterruptType t;

    pinindex = icemu_pinlist_find(&pin->chip->pins, pin);
    if (pinindex >= 0) {
        mcu = (ICeMCU *)pin->chip->logical_unit;
        if (mcu->interrupts[pinindex].func != NULL) {
            t = mcu->interrupts[pinindex].type;
            if ((t == ICE_INTERRUPT_ON_BOTH) || ((t == ICE_INTERRUPT_ON_RISING) == pin->high)) {
                mcu->interrupts[pinindex].usecs_since_last = 0;
                mcu->interrupts[pinindex].func();
            }
        }
    }
}

static void mcu_elapse(ICeChip *chip, time_t usecs)
{
    uint8_t i;
    ICeMCU *mcu;

    mcu = (ICeMCU *)chip->logical_unit;
    mcu->usecs_since_last_run += usecs;
    for (i = 0; i < chip->pins.count; i++) {
        // interrupt array is sparse. We don't break loop on NULL.
        if (mcu->interrupts[i].func != NULL) {
            mcu_interrupt_elapse(&mcu->interrupts[i], usecs);
        }
    }
    for (i = 0; i < ICE_MAX_TIMERS; i++) {
        if (mcu->timers[i].func == NULL) {
            break;
        }
        mcu_timer_elapse(&mcu->timers[i], usecs);
    }
    if (!mcu->in_runloop && (mcu->runloop != NULL) && (mcu->usecs_since_last_run >= mcu->resolution)) {
        mcu->usecs_since_last_run = 0;
        mcu->in_runloop = true;
        mcu->runloop();
        mcu->in_runloop = false;
    }
}

static ICeMCU* mcu_new(ICeChip *chip, const char **codes)
{
    ICeMCU *mcu;
    uint8_t count;
    uint8_t i;

    count = icemu_util_chararray_count(codes);
    mcu = (ICeMCU *)malloc(sizeof(ICeMCU));
    mcu->in_runloop = false;
    mcu->runloop = NULL;
    mcu->resolution = icemu_sim_resolution();
    mcu->usecs_since_last_run = 0;
    memset(mcu->interrupts, 0, sizeof(mcu->interrupts));
    memset(mcu->timers, 0, sizeof(mcu-> timers));
    icemu_chip_init(chip, (void *)mcu, mcu_pinchange, count);
    chip->elapse_func = mcu_elapse;
    for (i = 0; i < count; i++) {
        icemu_chip_addpin(chip, codes[i], false);
    }
    return mcu;
}

/* Public */
void icemu_mcu_set_runloop(ICeChip *chip, ICeRunloopFunc *runloop)
{
    ICeMCU *mcu;

    mcu = (ICeMCU *)chip->logical_unit;
    mcu->runloop = runloop;
}

void icemu_mcu_add_interrupt(
    ICeChip *chip, ICePin *pin, ICeMCUInterruptType type, ICeInterruptFunc *interrupt)
{
    int pinindex;
    ICeMCU *mcu;

    pinindex = icemu_pinlist_find(&chip->pins, pin);
    if (pinindex >= 0) {
        mcu = (ICeMCU *)chip->logical_unit;
        mcu->interrupts[pinindex].type = type;
        mcu->interrupts[pinindex].pin = pin;
        mcu->interrupts[pinindex].func = interrupt;
    }
}

void icemu_mcu_add_timer(ICeChip *chip, time_t every_usecs, ICeTimerFunc *timer_func)
{
    uint8_t i;
    ICeMCU *mcu;

    mcu = (ICeMCU *)chip->logical_unit;
    for (i = 0; i < ICE_MAX_TIMERS; i++) {
        if (mcu->timers[i].func == NULL) {
            mcu->timers[i].every = every_usecs;
            mcu->timers[i].func = timer_func;
            break;
        }
    }
}

void icemu_ATtiny_init(ICeChip *chip)
{
    const char * codes[] = {"PB0", "PB1", "PB2", "PB3", "PB4", "PB5", NULL};

    mcu_new(chip, codes);
}
