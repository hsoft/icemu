#include <avr/io.h>
#include "layer.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define USECS_PER_SECOND 1000000UL

void pinset(bool high)
{
    if (high) {
        sbi(PORTB, 1);
    } else {
        cbi(PORTB, 1);
    }
}

void pinoutputmode()
{
    sbi(DDRB, 1);
}

bool set_timer0_target(unsigned long usecs)
{
    unsigned long ticks;
    unsigned char prescaler_shifts[5] = {0, 3, 6, 8, 10};
    unsigned char prescaler_bits[5] = {0b001, 0b010, 0b011, 0b100, 0b101};
    unsigned char prescaler_index;

    if (usecs == 0) {
        // Stop the timer.
        TCCR0B &= 0b11111000;
        OCR0A = 0;
        return true;
    }

    if (F_CPU >= USECS_PER_SECOND) {
        ticks = (F_CPU / USECS_PER_SECOND) * usecs;
    } else {
        ticks = (F_CPU * usecs) / USECS_PER_SECOND;
        if (ticks == 0) {
            ticks = 1;
        }
    }

    for (prescaler_index=0; prescaler_index<=5; prescaler_index++) {
        if (ticks >> prescaler_shifts[prescaler_index] <= 0xff) {
            break;
        }
    }
    if (prescaler_index > 5) {
        return false;
    }
    // Set CS10, CS11 and CS12 according to our selected prescaler bits
    TCCR0B &= 0b11111000;
    TCCR0B |= prescaler_bits[prescaler_index];
    OCR0A = ticks >> prescaler_shifts[prescaler_index];
    return true;
}

void set_timer0_mode_to_interrupt()
{
    // Enable CTC mode
    sbi(TCCR0A, WGM01);
    // Enable interrupt on compare match on OCR0A.
    sbi(TIMSK, OCIE0A);
    TCNT0 = 0;
}

