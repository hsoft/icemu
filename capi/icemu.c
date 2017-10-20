#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#include "icemu.h"

#define MAX_5BITS 0x1f
#define MAX_TIMERS 10
#define MAX_INTERRUPTS 10

// 1 tick == ICEMU_TIME_RESOLUTION usecs
static unsigned long current_ticks = 0;
static unsigned long timers_resolution[MAX_TIMERS] = {0}; // in ticks. 0 = disabled
static unsigned long timers_counter[MAX_TIMERS] = {0}; // in ticks. 0 = threshold reached, time to set flag
static bool timers_flag[MAX_TIMERS] = {0}; // timer triggered
static unsigned char  interrupts_flag[MAX_INTERRUPTS] = {0}; // interrupt triggered

static void tick()
{
    unsigned char i;

    current_ticks++;
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

static void interrupt(unsigned char msg)
{
    msg &= MAX_5BITS;
    if (msg < MAX_INTERRUPTS) {
        interrupts_flag[msg]++;
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

static bool process_message(unsigned char msg) {
    switch (msg & 0xe0) {
        case ICEMU_RECV_TICK:
            tick();
            return true;
        case ICEMU_RECV_INTERRUPT:
            interrupt(msg);
            return true;
        default:
            return false;
    }
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
        // We have to call process_message() in case we don't receive our response right away
        if (!process_message(r)) {
            return (r & 0xe0) == ICEMU_RECV_PINHIGH;
        }
    }
}

void icemu_delay_us(unsigned int us)
{
    unsigned long target;

    target = us / ICEMU_TIME_RESOLUTION;
    if (target == 0) {
        target = 1;
    }
    target += current_ticks;
    while (current_ticks < target) {
        icemu_process_messages();
    }
}

void icemu_delay_ms(unsigned int ms)
{
    icemu_delay_us(ms * 1000);
}

bool icemu_check_interrupt(unsigned char interrupt_id)
{
    if (interrupt_id >= MAX_INTERRUPTS) {
        return false;
    }
    if (interrupts_flag[interrupt_id] > 0) {
        interrupts_flag[interrupt_id]--;
        return true;
    } else {
        return false;
    }
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

void icemu_process_messages()
{
    unsigned char r;

    r = readchar();
    assert(process_message(r));
}
