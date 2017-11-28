#include <stdio.h>
#include <stdint.h>
#include <CUnit/CUnit.h>

#include "../src/icemu.h"

static void test_IO()
{
    Chip chip;
    BinaryCounter *bc;
    int expected = 12;
    int i;

    icemu_SN74F161AN_init(&chip);
    bc = (BinaryCounter *)chip.logical_unit;
    for (i = 0; i < expected; i++) {
        icemu_pin_set(bc->clock, false);
        icemu_pin_set(bc->clock, true);
    }
    CU_ASSERT_EQUAL(icemu_util_get_binary_value(&bc->outputs), expected);
}

// When the input is oscillating, we end up with outputs that are oscillating at
// various rates. Pin 1 is going to be freq/2, Pin2, freq/4 and so on.
void test_oscillate()
{
    Chip chip;
    ICePin pin;
    int freq = 1000 * 1000;

    icemu_pin_init(&pin, NULL, "FOO", true);
    icemu_pin_set_oscillating_freq(&pin, freq);
    icemu_SN74F161AN_init(&chip);
    icemu_pin_wireto(&pin, icemu_chip_getpin(&chip, "CLK"));

    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "QA")->oscillating_freq, freq / 2);
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "QB")->oscillating_freq, freq / 4);
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "QC")->oscillating_freq, freq / 8);
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "QD")->oscillating_freq, freq / 16);
}

void test_counter_init()
{
    CU_pSuite s;

    s = CU_add_suite("Counters", NULL, NULL);
    CU_ADD_TEST(s, test_IO);
    CU_ADD_TEST(s, test_oscillate);
}

