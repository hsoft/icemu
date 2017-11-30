#include <stdio.h>
#include <unistd.h>
#include "layer.h"
#include "../../src/icemu.h"

static ICeChip mcu;
static ICeChip osc_mcu;

/* main.c decl */
void mmcu_setup();
void mmcu_loop();
void osc_setup();
void osc_loop();
void osc_int0_interrupt();
void osc_timer0_interrupt();

/* layer impl */
void osc_setup_interrupt()
{
    icemu_mcu_add_interrupt(
        &osc_mcu, icemu_chip_getpin(&osc_mcu, "PB0"), ICE_INTERRUPT_ON_RISING, osc_int0_interrupt);
}

bool osc_set_timer0_target(unsigned long usecs)
{
    icemu_mcu_add_timer(&osc_mcu, usecs, osc_timer0_interrupt);
    return true;
}

void osc_set_timer0_mode_to_interrupt()
{
}

void mmcu_pinhigh()
{
    icemu_pin_set(icemu_chip_getpin(&mcu, "PB1"), true);
}

void mmcu_pinlow()
{
    icemu_pin_set(icemu_chip_getpin(&mcu, "PB1"), false);
}

void mmcu_pinoutputmode()
{
    icemu_chip_getpin(&mcu, "PB1")->output = true;
}

void _delay_us(unsigned int us)
{
    icemu_sim_delay(us);
}

/* Simulation */

int main()
{
    icemu_sim_init();
    icemu_ATtiny_init(&mcu);
    // 43us duration because of our 42us delay;
    // We need to let the simulation a little room, time-wise. If you set "duration" to 42, the
    // result will be weird.
    icemu_mcu_set_runloop(&mcu, mmcu_loop, 43);
    icemu_ATtiny_init(&osc_mcu);

    icemu_pin_wireto(icemu_chip_getpin(&mcu, "PB1"), icemu_chip_getpin(&osc_mcu, "PB0"));
    mmcu_setup();
    osc_setup();
    icemu_ui_add_element("MCU 1:", &mcu);
    icemu_ui_add_element("MCU 2:", &osc_mcu);
    icemu_sim_run();
    return 0;
}
