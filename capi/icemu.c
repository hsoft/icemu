#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "icemu.h"

#define MAX_5BITS 0x1f

void icemu_pinset(unsigned char pin, bool high)
{
    unsigned char msg;

    msg = high ? ICEMU_RECV_PINHIGH : ICEMU_SEND_PINLOW;
    putchar(msg | (pin & MAX_5BITS));
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
