#include <stdbool.h>
#ifndef SIMULATION
#include <avr/io.h>
#include <util/delay.h>
#endif

#include "layer.h"

#define BLINK_DELAY_US 42

static bool flag;

void mmcu_setup()
{
    flag = false;
    mmcu_pinoutputmode();
}

void mmcu_loop()
{
    if (flag) {
        mmcu_pinhigh();
    } else {
        mmcu_pinlow();
    }
    flag = !flag;
    _delay_us(BLINK_DELAY_US);
}

#ifndef SIMULATION
int main()
{
    mmcu_setup();
    while(1) {
        mmcu_loop();
    }
}
#endif
