#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "mcu.h"
#include "util.h"

// this is the number of usecs that elapses at each runloop pass. If our simulation does a pass
// faster than this, we wait a little bit before continuing.
#define MCU_TIME_RESOLUTION 50


static void mcu_pinchange(Pin *pin)
{
    int pinindex;
    MCU *mcu;

    pinindex = icemu_pinlist_find(&pin->chip->pins, pin);
    if (pinindex >= 0) {
        mcu = (MCU *)pin->chip->logical_unit;
        if (mcu->interrupts[pinindex] != NULL) {
            mcu->interrupts[pinindex]();
        }
    }
}

static MCU* mcu_new(Chip *chip, char **codes)
{
    MCU *mcu;
    uint8_t count;
    uint8_t i;

    count = icemu_util_chararray_count(codes);
    mcu = (MCU *)malloc(sizeof(MCU));
    memset(mcu->interrupts, 0, sizeof(InterruptFunc) * MAX_INTERRUPTS);
    icemu_chip_init(chip, (void *)mcu, mcu_pinchange, count);
    for (i = 0; i < count; i++) {
        icemu_chip_addpin(chip, codes[i], false, false);
    }
    return mcu;
}

void icemu_mcu_add_interrupt(Chip *chip, Pin *pin, InterruptFunc interrupt)
{
    int pinindex;
    MCU *mcu;

    pinindex = icemu_pinlist_find(&chip->pins, pin);
    if (pinindex >= 0) {
        mcu = (MCU *)chip->logical_unit;
        mcu->interrupts[pinindex] = interrupt;
    }
}

void icemu_ATtiny_init(Chip *chip)
{
    char * codes[] = {"PB0", "PB1", "PB2", "PB3", "PB4", "PB5", NULL};

    mcu_new(chip, codes);
}
