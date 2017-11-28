#include <CUnit/CUnit.h>

#include "../src/icemu.h"

void test_fmthertz()
{
    char s[10];

    icemu_util_fmthertz(s, 42);
    CU_ASSERT_STRING_EQUAL(s, "42Hz");
    icemu_util_fmthertz(s, 1234);
    CU_ASSERT_STRING_EQUAL(s, "1kHz");
    icemu_util_fmthertz(s, 987654321UL);
    CU_ASSERT_STRING_EQUAL(s, "987MHz");
}

void test_util_init()
{
    CU_pSuite s;

    s = CU_add_suite("Util", NULL, NULL);
    CU_ADD_TEST(s, test_fmthertz);
}

