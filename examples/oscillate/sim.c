#include <stdio.h>
#include <unistd.h>
#include "../../src/icemu.h"
#include "../../src/util.h"

static ICeChip mcu;
static ICeChip cnt;
static ICeChip cnt2;
static ICeChip dec;
static ICePin oscillator;

/* main.c decl */
void setup();
void int0_interrupt();
void timer0_interrupt();

/* layer impl */
void setup_interrupt()
{
    icemu_mcu_add_interrupt(
        &mcu, icemu_chip_getpin(&mcu, "PB0"), ICE_INTERRUPT_ON_RISING, int0_interrupt);
}

bool set_timer0_target(unsigned long usecs)
{
    icemu_mcu_add_timer(&mcu, usecs, timer0_interrupt);
    return true;
}

void set_timer0_mode_to_interrupt()
{
}

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
    icemu_ATtiny_init(&mcu);

    icemu_pin_wireto(icemu_chip_getpin(&cnt, "CLK"), &oscillator);
    icemu_pin_wireto(icemu_chip_getpin(&cnt, "QD"), icemu_chip_getpin(&dec, "A"));
    icemu_pin_set(icemu_chip_getpin(&dec, "C"), true);
    icemu_pin_wireto(icemu_chip_getpin(&cnt2, "CLK"), icemu_chip_getpin(&dec, "Y5"));
    icemu_pin_wireto(icemu_chip_getpin(&cnt2, "QD"), icemu_chip_getpin(&mcu, "PB0"));

    icemu_pin_set_oscillating_freq(&oscillator, 1000UL * 1000UL);

    setup();

    icemu_sim_init(50, NULL);
    icemu_ui_add_element("Counter 1:", &cnt);
    icemu_ui_add_element("Decoder:", &dec);
    icemu_ui_add_element("Counter 2:", &cnt2);
    icemu_ui_add_element("MCU:", &mcu);
    icemu_sim_add_chip(&mcu);
    icemu_sim_add_action('+', "(+) Faster", faster);
    icemu_sim_add_action('-', "(-) Slower", slower);
    icemu_sim_run();
    return 0;
}

