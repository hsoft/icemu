#include <stdio.h>
#include <unistd.h>

#include "icemu.h"

#define MAX_5BITS 0x1f

void icemu_pinlow(unsigned char pin)
{
    putchar(ICEMU_SEND_PINLOW | (pin & MAX_5BITS));
    fflush(stdout);
}

void icemu_pinhigh(unsigned char pin)
{
    putchar(ICEMU_SEND_PINHIGH | (pin & MAX_5BITS));
    fflush(stdout);
}

bool icemu_pinread(unsigned char pin)
{
    unsigned char r;

    putchar(ICEMU_SEND_PINREAD | (pin & MAX_5BITS));
    fflush(stdout);
    r = getchar();
    return (r & 0xe0) == ICEMU_RECV_PINHIGH;
}

void icemu_delay_us(unsigned int us)
{
    usleep(us);
}

void icemu_delay_ms(unsigned int ms)
{
    usleep(ms * 1000);
}
