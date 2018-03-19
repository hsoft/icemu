#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <CUnit/CUnit.h>

#include "../src/icemu.h"
#include "../src/util.h"

static void test_IO()
{
    // some decoders decode binary values to some hardcoded binary values,
    // for example, the SN7447A decodes to 7-segment digits.

    ICeChip chip;
    ICeDecoder *dec;
    uint8_t val;

    icemu_SN7447A_init(&chip);
    dec = (ICeDecoder *)chip.logical_unit;

    icemu_pin_enable(icemu_chip_getpin(&chip, "A"), true);
    // selected value is a digital "1", binary value: an inverted 0b0110000
    val = icemu_util_get_binary_value(&dec->outputs);
    CU_ASSERT_EQUAL(val, 0b1001111);
}

void test_display_decoder_init()
{
    CU_pSuite s;

    s = CU_add_suite("Display Decoders", NULL, NULL);
    CU_ADD_TEST(s, test_IO);
}


