#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "decoder.h"
#include "chip.h"
#include "util.h"

/* Private */
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

static void decoder_update_output(Decoder *dec)
{
    uint8_t value;
    uint8_t i;

    if (icemu_pinlist_isenabled_all(&dec->enable_pins)) {
        value = get_binary_value(&dec->serial_pins);
    } else {
        value = 0xff;
    }
    for (i = 0; i < dec->outputs.count; i++) {
        icemu_pin_set(dec->outputs.pins[i], i != value);
    }
}

static void decoder_pinchange(Pin *pin)
{
    decoder_update_output((Decoder *)pin->chip->logical_unit);
}

static Decoder* decoder_new(
    Chip *chip,
    const char **input_codes,
    const char **output_codes,
    const char **serial_codes,
    const char **enable_codes)
{
    Decoder *dec;
    uint8_t input_count;
    uint8_t output_count;
    uint8_t total_count;

    input_count = icemu_util_chararray_count(input_codes);
    output_count = icemu_util_chararray_count(output_codes);
    total_count = input_count + output_count;

    dec = (Decoder *)malloc(sizeof(Decoder));
    icemu_chip_init(chip, (void *)dec, decoder_pinchange, total_count);
    icemu_chip_addpins(chip, &dec->inputs, input_codes, false);
    icemu_chip_addpins(chip, &dec->outputs, output_codes, true);
    icemu_pinlist_subset_of_existing(&dec->serial_pins, &dec->inputs, serial_codes);
    icemu_pinlist_subset_of_existing(&dec->enable_pins, &dec->inputs, enable_codes);
    return dec;
}

/* Public */
void icemu_SN74HC138_init(Chip *chip)
{
    const char * input_codes[] = {"A", "B", "C", "G1", "~G2A", "~G2B", NULL};
    const char * output_codes[] = {"Y0", "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7", NULL};
    const char * serial_codes[] = {"A", "B", "C", NULL};
    const char * enable_codes[] = {"G1", "G2A", "G2B", NULL};

    decoder_new(chip, input_codes, output_codes, serial_codes, enable_codes);
    icemu_chip_getpin(chip, "G1")->high = true;
    // ensure proper initial value
    decoder_update_output((Decoder *)chip->logical_unit);
}
