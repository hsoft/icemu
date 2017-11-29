#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "mcu.h"
#include "util.h"
#include "private.h"

/* Private */
static unsigned int mcu_interrupt_elapse(ICeMCUInterrupt *interrupt, time_t usecs)
{
    unsigned int freq, interrupt_count, usecs_per_interrupt;

    freq = interrupt->pin->oscillating_freq;
    if (freq > 0) {
        if (interrupt->type == ICE_INTERRUPT_ON_BOTH) {
            freq *= 2;
        }
        interrupt_count = (freq * usecs) / ICE_USECS_IN_SEC;
        if (interrupt_count > 0) {
            // easy: just interrupt "count" times.
            interrupt->usecs_since_last = 0;
            while (interrupt_count > 0) {
                interrupt->func();
                interrupt_count--;
            }
            return MAX(1, ICE_USECS_IN_SEC / freq);
        } else {
            // we don't interrupt on each tick. use usecs_since_last to know when to interrupt.
            interrupt->usecs_since_last += usecs;
            usecs_per_interrupt = ICE_USECS_IN_SEC / freq;
            if (usecs_per_interrupt < interrupt->usecs_since_last) {
                interrupt->usecs_since_last -= usecs_per_interrupt;
                interrupt->func();
            }
            return usecs_per_interrupt - interrupt->usecs_since_last;
        }
    } else {
        return 0;
    }
}

static unsigned int mcu_timer_elapse(ICeMCUTimer *timer, time_t usecs)
{
    timer->elapsed += usecs;
    if (timer->elapsed >= timer->every) {
        timer->elapsed -= timer->every;
        timer->func();
    }
    return timer->every - timer->elapsed;
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

static unsigned int mcu_elapse(ICeChip *chip, time_t usecs)
{
    uint8_t i;
    unsigned int result = ICE_MAX_CHIP_ELAPSE;
    unsigned int elapse;
    ICeMCU *mcu;

    mcu = (ICeMCU *)chip->logical_unit;
    mcu->usecs_since_last_run += usecs;
    for (i = 0; i < chip->pins.count; i++) {
        // interrupt array is sparse. We don't break loop on NULL.
        if (mcu->interrupts[i].func != NULL) {
            elapse =  mcu_interrupt_elapse(&mcu->interrupts[i], usecs);
            if (elapse > 0) {
                result = MIN(elapse, result);
            }
        }
    }
    for (i = 0; i < ICE_MAX_TIMERS; i++) {
        if (mcu->timers[i].func == NULL) {
            break;
        }
        elapse = mcu_timer_elapse(&mcu->timers[i], usecs);
        if (elapse > 0) {
            result = MIN(elapse, result);
        }
    }
    if (!mcu->in_runloop && (mcu->runloop != NULL) && (mcu->usecs_since_last_run >= mcu->runloop_duration)) {
        mcu->usecs_since_last_run = 0;
        mcu->in_runloop = true;
        mcu->runloop();
        mcu->in_runloop = false;
        result = MIN(result, mcu->runloop_duration);
    }
    if (result == ICE_MAX_CHIP_ELAPSE) {
        result = 0;
    }
    return result;
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
    mcu->runloop_duration = 0;
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
void icemu_mcu_set_runloop(ICeChip *chip, ICeRunloopFunc *runloop, unsigned int duration)
{
    ICeMCU *mcu;

    mcu = (ICeMCU *)chip->logical_unit;
    mcu->runloop = runloop;
    mcu->runloop_duration = duration;
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
