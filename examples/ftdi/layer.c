#include "layer.h"

void pinhigh()
{
    sbi(PORTB, 1);
}

void pinlow()
{
    cbi(PORTB, 1);
}

void pinoutputmode()
{
    sbi(DDRB, 1);
}

