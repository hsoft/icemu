#include <stdio.h>
#include <assert.h>

#include "icemu.h"

static void push_value(Chip *chip, uint8_t val)
{
    ShiftRegister *sr = (ShiftRegister *)chip->logical_unit;
    uint8_t i;
    bool flag;
    uint8_t count;

    count = icemu_shiftregister_outputcount(sr);
    for (i = 0; i < count; i++) {
        flag = val & (1 << (count - i - 1));
        icemu_pin_set(sr->serial1, flag);
        icemu_pin_set(sr->clock, false);
        icemu_pin_set(sr->clock, true);
        chip->pin_change_func(chip, sr->clock);
    }
}

static void assert_value(Chip *chip, uint8_t expected_value)
{
    ShiftRegister *sr = (ShiftRegister *)chip->logical_unit;
    uint8_t i;
    uint8_t count;
    uint8_t val = 0;

    count = icemu_shiftregister_outputcount(sr);
    for (i = 0; i < count; i++) {
        if (sr->outputs[i]->high) {
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
    push_value(&chip, expected);
    assert_value(&chip, expected);
}

int main()
{
    test_IO_unbuffered();
    printf("All tests passed.\n");
}
