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


void test_counter_init()
{
    CU_pSuite s;

    s = CU_add_suite("Counters", NULL, NULL);
    CU_ADD_TEST(s, test_IO);
}

