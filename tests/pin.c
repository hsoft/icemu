#include <CUnit/CUnit.h>

#include "../src/icemu.h"

static void test_wire_propagation_3pins()
{
    ICePin p1, p2, p3;

    icemu_pin_init(&p1, NULL, "P1", true);
    icemu_pin_init(&p2, NULL, "P2", false);
    icemu_pin_init(&p3, NULL, "P3", false);

    icemu_pin_wireto(&p1, &p2);
    icemu_pin_wireto(&p1, &p3);

    CU_ASSERT_FALSE(p1.high);
    CU_ASSERT_FALSE(p2.high);
    CU_ASSERT_FALSE(p3.high);

    icemu_pin_set(&p1, true); // propagation occurs

    CU_ASSERT_TRUE(p1.high);
    CU_ASSERT_TRUE(p2.high);
    CU_ASSERT_TRUE(p3.high);
}

// make sure that we can merge pins that both already have different wires
static void test_wire_merge()
{
    ICePin p1, p2, p3, p4;

    icemu_pin_init(&p1, NULL, "P1", true);
    icemu_pin_init(&p2, NULL, "P2", false);
    icemu_pin_init(&p3, NULL, "P3", false);
    icemu_pin_init(&p4, NULL, "P4", false);

    icemu_pin_wireto(&p1, &p2);
    icemu_pin_wireto(&p3, &p4);
    icemu_pin_wireto(&p1, &p3);

    CU_ASSERT_FALSE(p1.high);
    CU_ASSERT_FALSE(p2.high);
    CU_ASSERT_FALSE(p3.high);
    CU_ASSERT_FALSE(p4.high);

    icemu_pin_set(&p1, true); // propagation occurs

    CU_ASSERT_TRUE(p1.high);
    CU_ASSERT_TRUE(p2.high);
    CU_ASSERT_TRUE(p3.high);
    CU_ASSERT_TRUE(p4.high);
}

// Oscillation properly propagates through wires and an oscillating pin
// is always considered high.
void test_oscillate_wire_high()
{
    ICePin p1, p2;

    icemu_pin_init(&p1, NULL, "P1", true);
    icemu_pin_init(&p2, NULL, "P2", false);
    icemu_pin_wireto(&p1, &p2);
    icemu_pin_set_oscillating_freq(&p1, 42);
    CU_ASSERT_EQUAL(p1.oscillating_freq, 42);
    CU_ASSERT_TRUE(p1.high);
    CU_ASSERT_EQUAL(p2.oscillating_freq, 42);
    CU_ASSERT_TRUE(p2.high);
}

void test_pin_init()
{
    CU_pSuite s;

    s = CU_add_suite("Pins", NULL, NULL);
    CU_ADD_TEST(s, test_wire_propagation_3pins);
    CU_ADD_TEST(s, test_wire_merge);
    CU_ADD_TEST(s, test_oscillate_wire_high);
}
