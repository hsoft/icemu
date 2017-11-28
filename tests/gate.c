#include <stdio.h>
#include <stdint.h>
#include <CUnit/CUnit.h>

#include "../src/icemu.h"

static void test_NOR()
{
    Chip chip;

    icemu_CD4001B_init(&chip);
    // Initial state: all input low -> output high
    CU_ASSERT_FALSE(icemu_chip_getpin(&chip, "A")->high);
    CU_ASSERT_TRUE(icemu_chip_getpin(&chip, "J")->high);
    // Any input goes high: output low
    icemu_pin_set(icemu_chip_getpin(&chip, "A"), true);
    CU_ASSERT_FALSE(icemu_chip_getpin(&chip, "J")->high);
    // Other inputs on the chip not affected
    CU_ASSERT_TRUE(icemu_chip_getpin(&chip, "M")->high);
}

static void test_sr_latch()
{
    // let's implement an S-R latch!
    // https://en.wikipedia.org/wiki/Flip-flop_(electronics)
    Chip chip;
    Pin *d, *j, *b, *k, *c, *a;

    icemu_CD4001B_init(&chip);

    d = icemu_chip_getpin(&chip, "D");
    j = icemu_chip_getpin(&chip, "J");
    b = icemu_chip_getpin(&chip, "B");
    k = icemu_chip_getpin(&chip, "K");
    c = icemu_chip_getpin(&chip, "C");
    a = icemu_chip_getpin(&chip, "A");
    icemu_pin_wireto(d, j);
    icemu_pin_wireto(b, k);

    // Initial state: unknown! but what we know is that J == ~K
    CU_ASSERT_NOT_EQUAL(j->high, k->high);

    // set J through C
    icemu_pin_set(c, true);
    icemu_pin_set(c, false);
    CU_ASSERT_TRUE(j->high);
    CU_ASSERT_FALSE(k->high);

    // set K through A
    icemu_pin_set(a, true);
    icemu_pin_set(a, false);
    CU_ASSERT_TRUE(k->high);
    CU_ASSERT_FALSE(j->high);
}

static void test_INVERT()
{
    Chip chip;

    icemu_SN74HC14_init(&chip);
    // Initial state: all input low -> output high
    CU_ASSERT_TRUE(icemu_chip_getpin(&chip, "1Y")->high);
    // Input high -> output low
    icemu_pin_set(icemu_chip_getpin(&chip, "1A"), true);
    CU_ASSERT_FALSE(icemu_chip_getpin(&chip, "1Y")->high);
    // Other inputs on the chip not affected
    CU_ASSERT_TRUE(icemu_chip_getpin(&chip, "6Y")->high);
}

static void test_NOR_oscillate()
{
    Chip chip;
    Pin pin;
    int freq = 4242;

    icemu_CD4001B_init(&chip);
    icemu_pin_init(&pin, NULL, "FOO", true);
    icemu_pin_set_oscillating_freq(&pin, freq);

    icemu_pin_wireto(&pin, icemu_chip_getpin(&chip, "A"));
    // Because the position of pin_B makes a positive outcome for pin_J possible, it's oscillating.
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "J")->oscillating_freq, freq);

    icemu_pin_set(icemu_chip_getpin(&chip, "B"), true);
    // Because the position of pin_B makes a positive outcome for pin_J impossible, it's not oscillating.
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "J")->oscillating_freq, 0);
    CU_ASSERT_FALSE(icemu_chip_getpin(&chip, "J")->high);
}

static void test_INVERT_oscillate()
{
    Chip chip;
    Pin pin;
    int freq = 4242;

    icemu_SN74HC14_init(&chip);
    icemu_pin_init(&pin, NULL, "FOO", true);
    icemu_pin_set_oscillating_freq(&pin, freq);

    icemu_pin_wireto(&pin, icemu_chip_getpin(&chip, "1A"));
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "1Y")->oscillating_freq, freq);
    CU_ASSERT_EQUAL(icemu_chip_getpin(&chip, "2Y")->oscillating_freq, 0);
}

void test_gate_init()
{
    CU_pSuite s;

    s = CU_add_suite("Gates", NULL, NULL);
    CU_ADD_TEST(s, test_NOR);
    CU_ADD_TEST(s, test_sr_latch);
    CU_ADD_TEST(s, test_INVERT);
    CU_ADD_TEST(s, test_NOR_oscillate);
    CU_ADD_TEST(s, test_INVERT_oscillate);
}


