#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "decoder.h"
#include "chip.h"
#include "util.h"

/* Private */
static void decoder_update_output(ICeDecoder *dec)
{
    uint8_t value;
    uint8_t i;

    value = icemu_util_get_binary_value(&dec->serial_pins);
    for (i = 0; i < dec->outputs.count; i++) {
        icemu_pin_set(dec->outputs.pins[i], i != value);
    }
}

// determining oscillating behavior in a decoder when considering the possibility
// of more than one input pin oscillating quickly becomes complicated: the output
// is practically random. Let's just be statistical and consider that each possible
// outputs in the oscillating range is going to get an equal part of the output time.
// we use the maximum frequency value among input pins.
static void decoder_oscillate(ICeDecoder *dec)
{
    ICePin *p;
    unsigned int freq = icemu_pinlist_oscillating_freq(&dec->serial_pins);
    uint16_t base_value = icemu_util_get_binary_value(&dec->serial_pins);
    unsigned int oscillating_count = 0;
    unsigned int oscillating_mask = 0;
    int i, j;
    unsigned int match;

    // Compute the output freq, that is, divide by two for each oscillating pin
    for (i = 0; i < dec->serial_pins.count; i++) {
        if (dec->serial_pins.pins[i]->oscillating_freq > 0) {
            oscillating_count++;
            oscillating_mask |= 1 << i;
        }
    }
    freq /= (1 << (oscillating_count - 1));
    for (i = 0; i < dec->outputs.count; i++) {
        // how to determine if an output pin is part of the oscillating mask?
        // take the binary value of the index and see if its possible to attain that index
        // with any combination of bits within the oscillating_mask applied to base_value
        p = dec->outputs.pins[i];
        if ((i & base_value) != i) {
            // index is never selected
            icemu_pin_set(p, true);
            continue;
        }
        match = base_value;
        // now let's take "match" and try to bring it to "i" through toggling pins in
        // "oscillating_mask". If we can, our pin is oscillating.
        for (j = 0; j < dec->serial_pins.count; j++) {
            if (match == i) break;
            if (oscillating_mask & (1 << j)) {
                if (!(i & (1 << j))) {
                    match &= ~(1 << j);
                }
            }
        }
        if (match == i) {
            icemu_pin_set_oscillating_freq(dec->outputs.pins[i], freq);
        } else {
            // pin is never selected
            icemu_pin_set(p, true);
        }
    }
}

static void decoder_pinchange(ICePin *pin)
{
    ICeDecoder *dec = pin->chip->logical_unit;

    if (!icemu_pinlist_isenabled_all(&dec->enable_pins)) {
        icemu_pinlist_set_all(&dec->outputs, true);
    } else if (icemu_pinlist_oscillating_freq(&dec->serial_pins)) {
        decoder_oscillate(dec);
    } else {
        decoder_update_output(dec);
    }
}

static ICeDecoder* decoder_new(
    ICeChip *chip,
    const char **input_codes,
    const char **output_codes,
    const char **serial_codes,
    const char **enable_codes)
{
    ICeDecoder *dec;
    uint8_t input_count;
    uint8_t output_count;
    uint8_t total_count;

    input_count = icemu_util_chararray_count(input_codes);
    output_count = icemu_util_chararray_count(output_codes);
    total_count = input_count + output_count;

    dec = (ICeDecoder *)malloc(sizeof(ICeDecoder));
    icemu_chip_init(chip, (void *)dec, decoder_pinchange, total_count);
    icemu_chip_addpins(chip, &dec->inputs, input_codes, false);
    icemu_chip_addpins(chip, &dec->outputs, output_codes, true);
    icemu_pinlist_subset_of_existing(&dec->serial_pins, &dec->inputs, serial_codes);
    icemu_pinlist_subset_of_existing(&dec->enable_pins, &dec->inputs, enable_codes);
    return dec;
}

/* Public */
void icemu_SN74HC138_init(ICeChip *chip)
{
    const char * input_codes[] = {"A", "B", "C", "G1", "~G2A", "~G2B", NULL};
    const char * output_codes[] = {"Y0", "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7", NULL};
    const char * serial_codes[] = {"A", "B", "C", NULL};
    const char * enable_codes[] = {"G1", "G2A", "G2B", NULL};

    decoder_new(chip, input_codes, output_codes, serial_codes, enable_codes);
    icemu_chip_getpin(chip, "G1")->high = true;
    // ensure proper initial value
    decoder_update_output((ICeDecoder *)chip->logical_unit);
}
