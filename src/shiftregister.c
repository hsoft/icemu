#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "shiftregister.h"
#include "chip.h"
#include "util.h"

/* Private */
static void shiftregister_update_output(ShiftRegister *sr)
{
    if ((sr->enable_pin == NULL) || icemu_pin_isenabled(sr->enable_pin)) {
        icemu_util_set_binary_value(&sr->outputs, sr->buffer);
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
        if (sr->buffer_pin == NULL) { //unbuffered, update now
            shiftregister_update_output(sr);
        }
    } else if ((pin == sr->buffer_pin) && (pin->high)) {
        shiftregister_update_output(sr);
    } else if (pin == sr->enable_pin) {
        if (icemu_pin_isenabled(pin)) {
            shiftregister_update_output(sr);
        } else {
            icemu_util_set_binary_value(&sr->outputs, 0);
        }
    }
}

static ShiftRegister* shiftregister_new(Chip *chip, const char **input_codes, const char **output_codes)
{
    ShiftRegister *sr;
    uint8_t icount;
    uint8_t ocount;
    uint8_t i;

    icount = icemu_util_chararray_count(input_codes);
    ocount = icemu_util_chararray_count(output_codes);
    sr = (ShiftRegister *)malloc(sizeof(ShiftRegister));
    icemu_chip_init(chip, (void *)sr, shiftregister_pinchange, icount + ocount);
    sr->buffer = 0;
    sr->clock = NULL;
    sr->serial1 = NULL;
    sr->serial2 = NULL;
    sr->buffer_pin = NULL;
    for (i = 0; i < icount; i++) {
        icemu_chip_addpin(chip, input_codes[i], false, false);
    }
    icemu_pinlist_init(&sr->outputs, ocount);
    for (i = 0; i < ocount; i++) {
        icemu_pinlist_add(&sr->outputs, icemu_chip_addpin(chip, output_codes[i], true, false));
    }
    return sr;
}

/* Public */
void icemu_CD74AC164_init(Chip *chip)
{
    ShiftRegister *sr;
    const char * input_codes[] = {"CP", "DS1", "DS2", NULL};
    const char * output_codes[] = {"Q0", "Q1", "Q2", "Q3", "Q4", "Q5", "Q6", "Q7", NULL};

    sr = shiftregister_new(chip, input_codes, output_codes);
    sr->clock = chip->pins.pins[0];
    sr->serial1 = chip->pins.pins[1];
    sr->serial2 = chip->pins.pins[2];
    sr->serial2->high = true;
}

void icemu_SN74HC595_init(Chip *chip)
{
    ShiftRegister *sr;
    const char * input_codes[] = {"SER", "SRCLK", "RCLK", "~OE", "~SRCLR", NULL};
    const char * output_codes[] = {"QA", "QB", "QC", "QD", "QE", "QF", "QG", "QH", NULL};

    sr = shiftregister_new(chip, input_codes, output_codes);
    sr->clock = chip->pins.pins[1];
    sr->serial1 = chip->pins.pins[0];
    sr->buffer_pin = chip->pins.pins[2];
    sr->enable_pin = chip->pins.pins[3];
}
