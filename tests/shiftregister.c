#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <CUnit/CUnit.h>

#include "../src/icemu.h"

static void push_value(ICeChip *chip, uint8_t val, bool buffered)
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

static void assert_value(ICeChip *chip, uint8_t expected_value)
{
    ShiftRegister *sr = (ShiftRegister *)chip->logical_unit;
    CU_ASSERT_EQUAL(icemu_util_get_binary_value(&sr->outputs), expected_value);
}

static void test_IO_unbuffered()
{
    ICeChip chip;
    uint8_t expected = 0x42;

    icemu_CD74AC164_init(&chip);
    push_value(&chip, expected, false);
    assert_value(&chip, expected);
}

static void test_IO_buffered()
{
    ICeChip chip;
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
    ICeChip chip;
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

static void test_reset()
{
    ICeChip chip;
    ShiftRegister *sr;
    uint8_t expected = 0x42;

    icemu_CD74AC164_init(&chip);
    sr = (ShiftRegister *)chip.logical_unit;

    push_value(&chip, expected, false);
    assert_value(&chip, expected);

    icemu_pin_enable(sr->reset_pin, true);
    assert_value(&chip, 0);

    // When reset is enabled, pushing tuff does nothing.
    push_value(&chip, expected, false);
    assert_value(&chip, 0);

    // Disabling reset doesn't bring our value back.
    icemu_pin_enable(sr->reset_pin, true);
    assert_value(&chip, 0);
}

static void test_clock_and_buffer_pins_wired()
{
    /* In real life, when SRCLK and RCLK are wired together, the favlue that is sent to the output
     * pins is *not* the one that is just being pushed by SRCLK, but the one before. We need to
     * reproduce that behavior in the simulation too.
     */
    ICeChip chip;
    ShiftRegister *sr;
    ICePin p;

    icemu_SN74HC595_init(&chip);
    sr = (ShiftRegister *)chip.logical_unit;
    icemu_pin_init(&p, NULL, "OUT", true);
    icemu_pin_wireto(&p, sr->clock);
    icemu_pin_wireto(&p, sr->buffer_pin);

    icemu_pin_set(sr->serial1, true);
    icemu_pin_set(&p, false);
    icemu_pin_set(&p, true);

    // SRCLK *just* pushed its value. RCLK didn't catch it.
    CU_ASSERT_FALSE(sr->outputs.pins[0]->high);

    icemu_pin_set(&p, false);
    icemu_pin_set(&p, true);

    // *now* it did.
    CU_ASSERT_TRUE(sr->outputs.pins[0]->high);
}

void test_shiftregister_init()
{
    CU_pSuite s;

    s = CU_add_suite("Shift Registers", NULL, NULL);
    CU_ADD_TEST(s, test_IO_unbuffered);
    CU_ADD_TEST(s, test_IO_buffered);
    CU_ADD_TEST(s, test_disable_doesnt_reset_buffers);
    CU_ADD_TEST(s, test_reset);
    CU_ADD_TEST(s, test_clock_and_buffer_pins_wired);
}
