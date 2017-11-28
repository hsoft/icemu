#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "counter.h"
#include "chip.h"
#include "util.h"

/* Private */
static void binarycounter_update_output(ICeBinaryCounter *bc)
{
    icemu_util_set_binary_value(&bc->outputs, bc->value);
}

static void binarycounter_oscillate(ICeBinaryCounter *bc)
{
    int i, freq;

    freq = bc->clock->oscillating_freq;
    for (i = 0; i < bc->outputs.count; i++) {
        freq /= 2;
        icemu_pin_set_oscillating_freq(bc->outputs.pins[i], freq);
    }
}

static void binarycounter_pinchange(ICePin *pin)
{
    ICeBinaryCounter *bc = (ICeBinaryCounter *)pin->chip->logical_unit;

    if (!icemu_pinlist_isenabled_all(&bc->enable_pins)) {
        return;
    }
    if ((pin == bc->clock) && (pin->high)) {
        if (pin->oscillating_freq == 0) {
            bc->value++;
            binarycounter_update_output(bc);
        } else {
            binarycounter_oscillate(bc);
        }
    }
}

static ICeBinaryCounter* binarycounter_new(
    ICeChip *chip,
    const char **input_codes,
    const char **output_codes,
    const char **enable_codes,
    const char *clock_code)
{
    ICeBinaryCounter *bc;
    uint8_t icount;
    uint8_t ocount;

    icount = icemu_util_chararray_count(input_codes);
    ocount = icemu_util_chararray_count(output_codes);
    bc = (ICeBinaryCounter *)malloc(sizeof(ICeBinaryCounter));
    bc->value = 0;
    icemu_chip_init(chip, (void *)bc, binarycounter_pinchange, icount + ocount);
    icemu_chip_addpins(chip, &bc->inputs, input_codes, false);
    icemu_chip_addpins(chip, &bc->outputs, output_codes, true);
    bc->clock = icemu_chip_getpin(chip, clock_code);
    icemu_pinlist_subset_of_existing(&bc->enable_pins, &bc->inputs, enable_codes);
    icemu_pinlist_enable_all(&bc->enable_pins, true);
    return bc;
}

/* Public */
void icemu_SN74F161AN_init(ICeChip *chip)
{
    const char * input_codes[] = {"CLK", "ENT", "ENP", NULL};
    const char * output_codes[] = {"QA", "QB", "QC", "QD", NULL};
    const char * enable_codes[] = {"ENT", "ENP", NULL};

    binarycounter_new(chip, input_codes, output_codes, enable_codes, "CLK");
}

