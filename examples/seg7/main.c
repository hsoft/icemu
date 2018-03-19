#include <stdint.h>
#include <stdio.h>
#ifndef SIMULATION
#include <avr/io.h>
#include <util/delay.h>
#endif
#include "layer.h"

#define SER_A PinB0
#define SER_B PinB1
#define SER_C PinB2
#define SER_D PinB3

static uint8_t current_digit;

static void decsend(uint8_t val)
{
    pinset(SER_A, val & (1 << 0));
    pinset(SER_B, val & (1 << 1));
    pinset(SER_C, val & (1 << 2));
    pinset(SER_D, val & (1 << 3));
}

void setup()
{
    current_digit = 0;
    pinoutputmode(SER_A);
    pinoutputmode(SER_B);
    pinoutputmode(SER_C);
    pinoutputmode(SER_D);
}

void loop()
{
    decsend(current_digit);
    current_digit++;
    if (current_digit == 10) {
        current_digit = 0;
    }
#ifdef SIMULATION
    icemu_sim_set_debug_value("segval", current_digit);
#endif
    _delay_ms(1000);
}

#ifndef SIMULATION
int main()
{
    while (1) {
        loop();
    }

    return 0;
}
#endif
