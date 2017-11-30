#include "layer.h"

void mmcu_pinhigh()
{
    sbi(PORTB, 1);
}

void mmcu_pinlow()
{
    cbi(PORTB, 1);
}

void mmcu_pinoutputmode()
{
    sbi(DDRB, 1);
}

