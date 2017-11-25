#include <CUnit/CUnit.h>

#include "../src/icemu.h"

static void test_wire_propagation_3pins()
{
    Pin p1, p2, p3;

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

void test_pin_init()
{
    CU_pSuite s;

    s = CU_add_suite("Pins", NULL, NULL);
    CU_ADD_TEST(s, test_wire_propagation_3pins);
}
