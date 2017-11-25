#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <CUnit/CUnit.h>

#include "../src/icemu.h"

static void push_value(Chip *chip, uint8_t val, bool buffered)
{
    ShiftRegister *sr = (ShiftRegister *)chip->logical_unit;
    uint8_t i;
    bool flag;

    for (i = 0; i < sr->outputs.count; i++) {
        flag = val & (1 << (sr->outputs.count - i - 1));
        icemu_pin_set(sr->serial1, flag);
        icemu_pin_set(sr->clock, false);
        icemu_pin_set(sr->clock, true);
    }
    if (buffered) {
        icemu_pin_set(sr->buffer_pin, false);
        icemu_pin_set(sr->buffer_pin, true);
    }
}

static void assert_value(Chip *chip, uint8_t expected_value)
{
    ShiftRegister *sr = (ShiftRegister *)chip->logical_unit;
    uint8_t i;
    uint8_t val = 0;

    for (i = 0; i < sr->outputs.count; i++) {
        if (sr->outputs.pins[i]->high) {
            val |= 1 << i;
        }
    }
    assert(val == expected_value);
}

static void test_IO_unbuffered()
{
    Chip chip;
    uint8_t expected = 0x42;

    icemu_CD74AC164_init(&chip);
    push_value(&chip, expected, false);
    assert_value(&chip, expected);
}

static void test_IO_buffered()
{
    Chip chip;
    ShiftRegister *sr;
    uint8_t expected = 0x42;

    icemu_SN74HC595_init(&chip);
    sr = (ShiftRegister *)chip.logical_unit;
    push_value(&chip, expected, false);
    assert_value(&chip, 0);
    icemu_pin_set(sr->buffer_pin, false);
    icemu_pin_set(sr->buffer_pin, true);
    assert_value(&chip, expected);
    push_value(&chip, expected+1, false);
    assert_value(&chip, expected);
}

static void test_disable_doesnt_reset_buffers()
{
    /* When the "enable" pin is disabled, we don't want to reset the buffer, only make the */
    /* output pin temporarily have low outputs.                                            */
    Chip chip;
    ShiftRegister *sr;
    uint8_t expected = 0x42;

    icemu_SN74HC595_init(&chip);
    sr = (ShiftRegister *)chip.logical_unit;
    push_value(&chip, expected, true);
    icemu_pin_enable(sr->enable_pin, false);
    assert_value(&chip, 0);

    /* also, test that clocking the buffer pin doesn't change the output while it's disabled! */
    icemu_pin_set(sr->buffer_pin, false);
    icemu_pin_set(sr->buffer_pin, true);
    assert_value(&chip, 0);

    icemu_pin_enable(sr->enable_pin, true);
    assert_value(&chip, expected);
}

void test_shiftregister_init()
{
    CU_pSuite s;

    s = CU_add_suite("Shift Registers", NULL, NULL);
    CU_ADD_TEST(s, test_IO_unbuffered);
    CU_ADD_TEST(s, test_IO_buffered);
    CU_ADD_TEST(s, test_disable_doesnt_reset_buffers);
}
