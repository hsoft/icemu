#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <CUnit/CUnit.h>

#include "../src/icemu.h"

static void assert_pin_is_selected(ICeChip *chip, uint8_t selected_pin)
{
    Decoder *dec = (Decoder *)chip->logical_unit;
    uint8_t i;

    for (i = 0; i < dec->outputs.count; i++) {
        CU_ASSERT_EQUAL(dec->outputs.pins[i]->high, (i != selected_pin));
    }
}

static void test_IO()
{
    ICeChip chip;
    Decoder *dec;

    icemu_SN74HC138_init(&chip);
    dec = (Decoder *)chip.logical_unit;

    // initially, a decoder has its first pin selected
    assert_pin_is_selected(&chip, 0);

    // setting the 2nd pin to true should give us a two. that means that
    // the 3rd output pin is selected.
    icemu_pin_set(dec->inputs.pins[1], true);
    assert_pin_is_selected(&chip, 2);
}

// Disabled decoders have all their outputs high
// a decoder is diabled when any of its enable pins is disabled
static void test_disabled()
{
    ICeChip chip;

    icemu_SN74HC138_init(&chip);

    icemu_pin_enable(icemu_chip_getpin(&chip, "G1"), false);
    assert_pin_is_selected(&chip, 0xff);
    icemu_pin_enable(icemu_chip_getpin(&chip, "G1"), true);
    assert_pin_is_selected(&chip, 0);
    icemu_pin_enable(icemu_chip_getpin(&chip, "G2A"), false);
    assert_pin_is_selected(&chip, 0xff);
}

static void test_oscillate()
{
    ICeChip chip;
    ICePin pin;
    int freq = 4242;

    icemu_SN74HC138_init(&chip);

    icemu_pin_init(&pin, NULL, "FOO", true);
    icemu_pin_set_oscillating_freq(&pin, freq);

    icemu_pin_wireto(&pin, icemu_chip_getpin(&chip, "B"));
    // 2 oscillating pins
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "Y0")->oscillating_freq, freq);
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "Y2")->oscillating_freq, freq);
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "Y1")->oscillating_freq, 0);
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "Y6")->oscillating_freq, 0);
    CU_ASSERT_TRUE(icemu_chip_getpin(&chip, "Y1")->high);

    icemu_pin_set(icemu_chip_getpin(&chip, "C"), true);
    // 2 oscillating pins, but higher ones
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "Y4")->oscillating_freq, freq);
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "Y6")->oscillating_freq, freq);
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "Y2")->oscillating_freq, 0);
    CU_ASSERT_TRUE(icemu_chip_getpin(&chip, "Y2")->high);

    icemu_pin_wireto(&pin, icemu_chip_getpin(&chip, "A"));
    // 4 oscillating pins
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "Y4")->oscillating_freq, freq / 2);
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "Y5")->oscillating_freq, freq / 2);
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "Y6")->oscillating_freq, freq / 2);
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "Y7")->oscillating_freq, freq / 2);
    CU_ASSERT_TRUE(icemu_chip_getpin(&chip, "Y0")->high);
}

void test_decoder_init()
{
    CU_pSuite s;

    s = CU_add_suite("Decoders", NULL, NULL);
    CU_ADD_TEST(s, test_IO);
    CU_ADD_TEST(s, test_disabled);
    CU_ADD_TEST(s, test_oscillate);
}

