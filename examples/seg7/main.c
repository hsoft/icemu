#include <stdint.h>
#include <stdio.h>
#ifndef SIMULATION
#include <avr/io.h>
#include <util/delay.h>
#endif
#include "layer.h"

#define SRCLK PinB0
#define SRSER PinB1
#define RCLK PinB2

// Least significant bit is on Q0
//               XABCDEGF
#define Seg7_0 0b01111101
#define Seg7_1 0b00110000
#define Seg7_2 0b01101110
#define Seg7_3 0b01111010
#define Seg7_4 0b00110011
#define Seg7_5 0b01011011
#define Seg7_6 0b01011111
#define Seg7_7 0b01110000
#define Seg7_8 0b01111111
#define Seg7_9 0b01111011

static uint8_t current_digit;

static void toggleclk(PinID pin)
{
    pinlow(pin);
    _delay_us(1);
    pinhigh(pin);
}

// LSB goes on q0
static void shiftsend(uint8_t val)
{
    char i;

    for (i=7; i>=0; i--) {
        pinset(SRSER, val & (1 << i));
        toggleclk(SRCLK);
    }
}

static void senddigit(uint8_t val)
{
    uint8_t digits[10] = {Seg7_0, Seg7_1, Seg7_2, Seg7_3, Seg7_4, Seg7_5, Seg7_6, Seg7_7, Seg7_8, Seg7_9};

    pinlow(RCLK);
    shiftsend(~digits[val]);
    pinhigh(RCLK);
}

void setup()
{
    current_digit = 0;
    pinoutputmode(SRSER);
    pinoutputmode(SRCLK);
    pinoutputmode(RCLK);
}

void loop()
{
    senddigit(current_digit);
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
