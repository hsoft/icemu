#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <CUnit/CUnit.h>

#include "../src/icemu.h"

static void test_dec_and_srs()
{
    // One decoder managing two SRs.
    // SRs' clock are connected to the decoder, but their DS1 is connected to the
    // same output pin.

    Chip dec, sr1, sr2;
    Pin pin;

    icemu_SN74HC138_init(&dec);
    icemu_CD74AC164_init(&sr1);
    icemu_CD74AC164_init(&sr2);
    icemu_pin_init(&pin, NULL, "PB4", true);
    icemu_pin_set(&pin, true);

    icemu_pin_wireto(icemu_chip_getpin(&sr1, "CP"), icemu_chip_getpin(&dec, "Y0"));
    icemu_pin_wireto(icemu_chip_getpin(&sr2, "CP"), icemu_chip_getpin(&dec, "Y1"));
    icemu_pin_wireto(icemu_chip_getpin(&sr1, "DS1"), &pin);
    icemu_pin_wireto(icemu_chip_getpin(&sr2, "DS1"), &pin);

    assert(!icemu_chip_getpin(&dec, "Y0")->high);

    // let's toggle SR1 CP by activating making Y0 (initially selected) high
    icemu_pin_set(icemu_chip_getpin(&dec, "B"), true); // Y2 is selected now (unconnected)

    assert(icemu_chip_getpin(&dec, "Y0")->high);
    assert(icemu_chip_getpin(&sr1, "Q0")->high);
    assert(!icemu_chip_getpin(&sr2, "Q0")->high);
    assert(!icemu_chip_getpin(&sr1, "Q1")->high);

    // SR2's turn now, toggle clock
    icemu_pin_set(icemu_chip_getpin(&dec, "A"), true); // Y3 is selected (unconnected)
    icemu_pin_set(icemu_chip_getpin(&dec, "B"), false); // Y1 is selected (low)
    icemu_pin_set(icemu_chip_getpin(&dec, "A"), false); // Y0 is selected (low). Y1 is high, triggering a clock on SR2

    assert(icemu_chip_getpin(&sr2, "Q0")->high);
    assert(!icemu_chip_getpin(&sr1, "Q1")->high);
}

void test_circuit_init()
{
    CU_pSuite s;

    s = CU_add_suite("Circuits", NULL, NULL);
    CU_ADD_TEST(s, test_dec_and_srs);
}

