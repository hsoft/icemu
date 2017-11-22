#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "shiftregister.h"
#include "chip.h"
#include "util.h"

static void shiftregister_updateoutputs(Chip *chip, ShiftRegister *sr)
{
    uint8_t i;

    for (i = 0; i < sr->outputs.count; i++) {
        icemu_pin_set(sr->outputs.pins[i], (sr->buffer >> i) & 0x1);
    }
}

static void shiftregister_pinchange(Pin *pin)
{
    ShiftRegister *sr = (ShiftRegister *)pin->chip->logical_unit;

    if ((pin == sr->clock) && (pin->high)) {
        sr->buffer = sr->buffer << 1;
        if (sr->serial1->high && (sr->serial2 == NULL || sr->serial2->high)) {
            sr->buffer |= 0x1;
        }
        shiftregister_updateoutputs(pin->chip, sr);
    }
}

static ShiftRegister* shiftregister_new(Chip *chip, bool isbuffered, char **output_codes)
{
    ShiftRegister *sr;
    uint8_t count;
    uint8_t total_count;
    uint8_t i;

    count = icemu_util_chararray_count(output_codes);
    total_count = count + 3; // clk + serial1 + serial2
    sr = (ShiftRegister *)malloc(sizeof(ShiftRegister));
    icemu_chip_init(chip, (void *)sr, shiftregister_pinchange, total_count);
    sr->buffer = 0;
    sr->isbuffered = false;
    sr->clock = NULL;
    sr->serial1 = NULL;
    sr->serial2 = NULL;
    icemu_pinlist_init(&sr->outputs, count);
    for (i = 0; i < count; i++) {
        icemu_pinlist_add(&sr->outputs, icemu_chip_addpin(chip, output_codes[i], true, false));
    }
    return sr;
}

void icemu_CD74AC164_init(Chip *chip)
{
    ShiftRegister *sr;
    char * output_codes[] = {"Q0", "Q1", "Q2", "Q3", "Q4", "Q5", "Q6", "Q7", NULL};

    sr = shiftregister_new(chip, false, output_codes);
    sr->clock = icemu_chip_addpin(chip, "SRCLK", false, false);
    sr->serial1 = icemu_chip_addpin(chip, "DS1", false, false);
    sr->serial2 = icemu_chip_addpin(chip, "DS2", false, false);
    sr->serial2->high = true;
}
