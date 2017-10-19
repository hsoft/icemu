#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#include "icemu.h"

#define MAX_5BITS 0x1f
#define MAX_TIMERS 10

// 1 tick == ICEMU_TIME_RESOLUTION usecs
static unsigned long timers_resolution[MAX_TIMERS] = {0}; // in ticks. 0 = disabled
static unsigned long timers_counter[MAX_TIMERS] = {0}; // in ticks. 0 = threshold reached, time to set flag
static bool timers_flag[MAX_TIMERS] = {0}; // timer triggered

static void tick()
{
    unsigned char i;

    for (i = 0; i < MAX_TIMERS; i++) {
        if (timers_resolution[i] > 0) {
            timers_counter[i]--;
            if (timers_counter[i] == 0) {
                timers_counter[i] = timers_resolution[i];
                timers_flag[i] = true;
            }
        }
    }
}

static unsigned char readchar()
{
    int c;

    c = getchar();
    if (c < 0) {
        // stdin broken, stop
        exit(1);
    }
    return c;
}

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
    while (1) {
        r = readchar();
        if ((r & 0xe0) == ICEMU_RECV_TICK) {
            // oh, we've received a tick while waiting for a response. just tick time...
            tick();
        } else {
            return (r & 0xe0) == ICEMU_RECV_PINHIGH;
        }
    }
}

void icemu_delay_us(unsigned int us)
{
    unsigned int ticks;

    ticks = us / ICEMU_TIME_RESOLUTION;
    if (ticks == 0) {
        ticks = 1;
    }
    for (; ticks > 0; ticks--) {
        icemu_tick_time();
    }
}

void icemu_delay_ms(unsigned int ms)
{
    icemu_delay_us(ms * 1000);
}

bool icemu_start_timer(unsigned char timer_id, unsigned long usecs)
{
    if (timer_id >= MAX_TIMERS) {
        return false;
    }
    timers_resolution[timer_id] = usecs / ICEMU_TIME_RESOLUTION;
    timers_counter[timer_id] = timers_resolution[timer_id];
    return true;
}

bool icemu_check_timer(unsigned char timer_id)
{
    if (timer_id >= MAX_TIMERS) {
        return false;
    }
    if (timers_flag[timer_id]) {
        timers_flag[timer_id] = false;
        return true;
    } else {
        return false;
    }
}

void icemu_tick_time()
{
    unsigned char r;

    r = readchar();
    assert((r & 0xe0) == ICEMU_RECV_TICK);
    tick();
}
