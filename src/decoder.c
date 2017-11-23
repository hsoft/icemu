#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "decoder.h"
#include "chip.h"
#include "util.h"

static uint8_t get_binary_value(PinList *pinlist)
{
    uint8_t result = 0;
    uint8_t i;

    for (i = 0; i < pinlist->count; i++) {
        if (pinlist->pins[i]->high) {
            result |= 1 << i;
        }
    }
    return result;
}

static void decoder_pinchange(Pin *pin)
{
    uint8_t value;
    uint8_t i;
    Decoder *dec = (Decoder *)pin->chip->logical_unit;

    value = get_binary_value(&dec->inputs);
    for (i = 0; i < dec->outputs.count; i++) {
        icemu_pin_set(dec->outputs.pins[i], i != value);
    }
}

static Decoder* decoder_new(Chip *chip, const char **input_codes, const char **output_codes)
{
    Decoder *dec;
    uint8_t i;
    uint8_t input_count;
    uint8_t output_count;
    uint8_t total_count;

    input_count = icemu_util_chararray_count(input_codes);
    output_count = icemu_util_chararray_count(output_codes);
    total_count = input_count + output_count;

    dec = (Decoder *)malloc(sizeof(Decoder));
    icemu_chip_init(chip, (void *)dec, decoder_pinchange, total_count);
    icemu_pinlist_init(&dec->inputs, input_count);
    for (i = 0; i < input_count; i++) {
        icemu_pinlist_add(&dec->inputs, icemu_chip_addpin(chip, input_codes[i], false, false));
    }
    icemu_pinlist_init(&dec->outputs, output_count);
    for (i = 0; i < output_count; i++) {
        icemu_pinlist_add(&dec->outputs, icemu_chip_addpin(chip, output_codes[i], true, false));
    }
    // ensure proper initial value
    decoder_pinchange(dec->inputs.pins[0]);
    return dec;
}

void icemu_SN74HC138_init(Chip *chip)
{
    const char * input_codes[] = {"A", "B", "C", NULL};
    const char * output_codes[] = {"Y0", "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7", NULL};

    decoder_new(chip, input_codes, output_codes);
}
