#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "display_decoder.h"
#include "chip.h"
#include "util.h"
#include "private.h"

/* Private */
static void decoder_update_output(ICeDisplayDecoder *dec)
{
    uint8_t index;
    uint8_t value;
    uint8_t i;

    index = icemu_util_get_binary_value(&dec->serial_pins);
    value = dec->value_table[index];
    icemu_util_set_binary_value(&dec->outputs, value);
}

static void decoder_pinchange(ICePin *pin)
{
    ICeDisplayDecoder *dec = pin->chip->logical_unit;

    decoder_update_output(dec);
}

static ICeDisplayDecoder* decoder_new(
    ICeChip *chip,
    const char **input_codes,
    const char **output_codes,
    const char **serial_codes,
    uint8_t *value_table)
{
    ICeDisplayDecoder *dec;
    uint8_t input_count;
    uint8_t output_count;
    uint8_t total_count;

    input_count = icemu_util_chararray_count(input_codes);
    output_count = icemu_util_chararray_count(output_codes);
    total_count = input_count + output_count;

    dec = (ICeDisplayDecoder *)malloc(sizeof(ICeDisplayDecoder));
    icemu_chip_init(chip, (void *)dec, decoder_pinchange, total_count);
    icemu_chip_addpins(chip, &dec->inputs, input_codes, false);
    icemu_chip_addpins(chip, &dec->outputs, output_codes, true);
    icemu_pinlist_subset_of_existing(
        &dec->serial_pins, &dec->inputs, serial_codes);
    dec->value_table = value_table;
    decoder_update_output(dec);
    return dec;
}

/* Public */

/* SN7447A is a 7-segment decoder with a table of 16 hardcoded values selected
 * from a 4-bit binary input.
 *
 * Values are low-enabled, hence the "~".
 */

static uint8_t SN7447A_value_table[16] = {
    //ABCDEFG
    ~0b1111110, // 0
    ~0b0110000, // 1
    ~0b1101101, // 2
    ~0b1111001, // 3
    ~0b0110011, // 4
    ~0b1011011, // 5
    ~0b0011111, // 6
    ~0b1110000, // 7
    ~0b1111111, // 8
    ~0b1110011, // 9
    ~0b0001101, // lower part of 2. matches 2, 6, 8
    ~0b0011001, // lower part of 5. matches 3, 5, 6, 8
    ~0b0100011, // upper part of 4. matches 4, 8, 9
    ~0b1001011, // 5 missing a branch. matches 5,8 and tailed 6, 9
    ~0b0001111, // 6 missing a branch. matches 6, 8
    ~0b0000000, // blank
};

void icemu_SN7447A_init(ICeChip *chip)
{
    const char * input_codes[] = {"A", "B", "C", "D", NULL};
    const char * output_codes[] = {"g", "f", "e", "d", "c", "b", "a", NULL};
    const char * serial_codes[] = {"A", "B", "C", "D", NULL};

    decoder_new(
        chip, input_codes, output_codes, serial_codes, SN7447A_value_table);
}

