#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "mcu.h"
#include "util.h"

/* Private */
static void mcu_timer_elapse(MCUTimer *timer, time_t usecs)
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
    MCU *mcu;
    MCUInterruptType t;

    pinindex = icemu_pinlist_find(&pin->chip->pins, pin);
    if (pinindex >= 0) {
        mcu = (MCU *)pin->chip->logical_unit;
        if (mcu->interrupts[pinindex].func != NULL) {
            t = mcu->interrupts[pinindex].type;
            if ((t == INTERRUPT_ON_BOTH) || ((t == INTERRUPT_ON_RISING) == pin->high)) {
                mcu->interrupts[pinindex].func();
            }
        }
    }
}

static void mcu_elapse(ICeChip *chip, time_t usecs)
{
    uint8_t i;
    MCU *mcu;

    mcu = (MCU *)chip->logical_unit;
    for (i = 0; i < MAX_TIMERS; i++) {
        if (mcu->timers[i].func == NULL) {
            break;
        }
        mcu_timer_elapse(&mcu->timers[i], usecs);
    }
}

static MCU* mcu_new(ICeChip *chip, const char **codes)
{
    MCU *mcu;
    uint8_t count;
    uint8_t i;

    count = icemu_util_chararray_count(codes);
    mcu = (MCU *)malloc(sizeof(MCU));
    memset(mcu->interrupts, 0, sizeof(MCUInterrupt) * MAX_INTERRUPTS);
    memset(mcu->timers, 0, sizeof(MCUTimer) * MAX_TIMERS);
    icemu_chip_init(chip, (void *)mcu, mcu_pinchange, count);
    chip->elapse_func = mcu_elapse;
    for (i = 0; i < count; i++) {
        icemu_chip_addpin(chip, codes[i], false);
    }
    return mcu;
}

/* Public */
void icemu_mcu_add_interrupt(
    ICeChip *chip, ICePin *pin, MCUInterruptType type, ICeInterruptFunc *interrupt)
{
    int pinindex;
    MCU *mcu;

    pinindex = icemu_pinlist_find(&chip->pins, pin);
    if (pinindex >= 0) {
        mcu = (MCU *)chip->logical_unit;
        mcu->interrupts[pinindex].type = type;
        mcu->interrupts[pinindex].func = interrupt;
    }
}

void icemu_mcu_add_timer(ICeChip *chip, time_t every_usecs, ICeTimerFunc *timer_func)
{
    uint8_t i;
    MCU *mcu;

    mcu = (MCU *)chip->logical_unit;
    for (i = 0; i < MAX_TIMERS; i++) {
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
