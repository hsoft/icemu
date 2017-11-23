#include <stdlib.h>
#include <stdint.h>

#include "mcu.h"
#include "util.h"

// this is the number of usecs that elapses at each runloop pass. If our simulation does a pass
// faster than this, we wait a little bit before continuing.
#define MCU_TIME_RESOLUTION 50


static void mcu_pinchange(Pin *pin)
{
}

static MCU* mcu_new(Chip *chip, char **codes)
{
    MCU *mcu;
    uint8_t count;
    uint8_t i;

    count = icemu_util_chararray_count(codes);
    mcu = (MCU *)malloc(sizeof(MCU));
    icemu_chip_init(chip, (void *)mcu, mcu_pinchange, count);
    for (i = 0; i < count; i++) {
        icemu_chip_addpin(chip, codes[i], false, false);
    }
    return mcu;
}

void icemu_ATtiny_init(Chip *chip)
{
    char * codes[] = {"PB0", "PB1", "PB2", "PB3", "PB4", "PB5", NULL};

    mcu_new(chip, codes);
}
