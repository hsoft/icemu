#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "../src/icemu.h"

static void assert_pin_is_selected(Chip *chip, uint8_t selected_pin)
{
    Decoder *dec = (Decoder *)chip->logical_unit;
    uint8_t i;

    for (i = 0; i < dec->outputs.count; i++) {
        assert(dec->outputs.pins[i]->high == (i != selected_pin));
    }
}

static void test_IO()
{
    Chip chip;
    Decoder *dec;

    icemu_SN74HC138_init(&chip);
    dec = (Decoder *)chip.logical_unit;
    // setting the 2nd pin to true should give us a two. that means that
    // the 3rd output pin is selected.
    icemu_pin_set(dec->inputs.pins[1], true);
    assert_pin_is_selected(&chip, 2);
}

void test_decoder_main()
{
    printf("Testing decoders\n");
    test_IO();
}

