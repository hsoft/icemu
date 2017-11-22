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

    for (i = 0; i < icemu_util_pincount(sr->outputs, MAX_SHIFTREGISTER_OUTPUTS); i++) {
        icemu_pin_set(sr->outputs[i], (sr->buffer >> i) & 0x1);
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

static ShiftRegister* shiftregister_new(bool isbuffered)
{
    ShiftRegister *sr;

    sr = (ShiftRegister *)malloc(sizeof(ShiftRegister));
    sr->buffer = 0;
    sr->isbuffered = false;
    sr->clock = NULL;
    sr->serial1 = NULL;
    sr->serial2 = NULL;
    memset(sr->outputs, 0, sizeof(Pin*) * MAX_SHIFTREGISTER_OUTPUTS);
    return sr;
}

void icemu_CD74AC164_init(Chip *chip)
{
    ShiftRegister *sr;
    char * output_code[] = {"Q0", "Q1", "Q2", "Q3", "Q4", "Q5", "Q6", "Q7"};
    uint8_t i;

    sr = shiftregister_new(false);
    chip->logical_unit = (void *)sr;
    chip->pin_change_func = shiftregister_pinchange;
    memset(chip->pins, 0, sizeof(Pin*) * MAX_PINS_PER_CHIP);
    sr->clock = icemu_chip_addpin(chip, "SRCLK", false, false);
    sr->serial1 = icemu_chip_addpin(chip, "DS1", false, false);
    sr->serial2 = icemu_chip_addpin(chip, "DS2", false, false);
    sr->serial2->high = true;
    for (i = 0; i < 8; i++) {
        sr->outputs[i] = icemu_chip_addpin(chip, output_code[i], true, false);
    }
}
