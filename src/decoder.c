#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "decoder.h"
#include "chip.h"
#include "util.h"

static uint8_t get_binary_value(Pin **pins, uint8_t count)
{
    uint8_t result = 0;
    uint8_t i;

    for (i = 0; i < count; i++) {
        if (pins[i]->high) {
            result |= 1 << i;
        }
    }
    return result;
}

static void decoder_pinchange(Pin *pin)
{
    uint8_t value;
    uint8_t input_count;
    uint8_t output_count;
    uint8_t i;
    Decoder *dec = (Decoder *)pin->chip->logical_unit;

    input_count = icemu_util_pincount(dec->inputs, MAX_DECODER_INPUTS);
    output_count = icemu_util_pincount(dec->outputs, MAX_DECODER_OUTPUTS);
    value = get_binary_value(dec->inputs, input_count);
    for (i = 0; i < output_count; i++) {
        icemu_pin_set(dec->outputs[i], i != value);
    }
}

static Decoder* decoder_new()
{
    Decoder *dec;

    dec = (Decoder *)malloc(sizeof(Decoder));
    memset(dec->inputs, 0, sizeof(Pin*) * MAX_DECODER_INPUTS);
    memset(dec->outputs, 0, sizeof(Pin*) * MAX_DECODER_OUTPUTS);
    return dec;
}

void icemu_SN74HC138_init(Chip *chip)
{
    Decoder *dec;
    char * input_code[] = {"A", "B", "C"};
    char * output_code[] = {"Y0", "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7"};
    uint8_t i;

    dec = decoder_new();
    icemu_chip_init(chip, (void *)dec, decoder_pinchange);
    for (i = 0; i < 3; i++) {
        dec->inputs[i] = icemu_chip_addpin(chip, input_code[i], false, false);
    }
    for (i = 0; i < 8; i++) {
        dec->outputs[i] = icemu_chip_addpin(chip, output_code[i], true, false);
    }
}
