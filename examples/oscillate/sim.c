#include <stdio.h>
#include <unistd.h>
#include "../../src/icemu.h"

static Chip cnt;
static Chip cnt2;
static Chip dec;
static ICePin oscillator;

/* Simulation */

static void faster()
{
    icemu_pin_set_oscillating_freq(&oscillator, oscillator.oscillating_freq * 10);
}

static void slower()
{
    icemu_pin_set_oscillating_freq(&oscillator, MAX(1, oscillator.oscillating_freq / 10));
}

int main()
{
    icemu_pin_init(&oscillator, NULL, "XTAL", true);
    icemu_SN74F161AN_init(&cnt);
    icemu_SN74F161AN_init(&cnt2);
    icemu_SN74HC138_init(&dec);

    icemu_pin_wireto(icemu_chip_getpin(&cnt, "CLK"), &oscillator);
    icemu_pin_wireto(icemu_chip_getpin(&cnt, "QD"), icemu_chip_getpin(&dec, "A"));
    icemu_pin_set(icemu_chip_getpin(&dec, "C"), true);
    icemu_pin_wireto(icemu_chip_getpin(&cnt2, "CLK"), icemu_chip_getpin(&dec, "Y5"));

    icemu_pin_set_oscillating_freq(&oscillator, 1000UL * 1000UL);

    icemu_sim_init(50, NULL);
    icemu_ui_add_element("Counter 1:", &cnt);
    icemu_ui_add_element("Decoder:", &dec);
    icemu_ui_add_element("Counter 2:", &cnt2);
    icemu_sim_add_action('+', "(+) Faster", faster);
    icemu_sim_add_action('-', "(-) Slower", slower);
    icemu_sim_run();
    return 0;
}

