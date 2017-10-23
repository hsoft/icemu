#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#include "icemu.h"

/* messages are sent through stdin/stdout. stdout carries:
 *
 * - output pin changes
 * - input pin read requests
 *
 * stdin carries:
 *
 * - input pin result
 * - interrupt (timer or pin change)
 *
 * Each message is one byte long. The first 3 bits are the message and
 * the rest is the argument (pin id, interrupt id, delay amount)
 */

#define MAX_5BITS 0x1f
#define MAX_TIMERS 10
#define MAX_PINS MAX_5BITS

#define ICEMU_TIME_RESOLUTION 50 // in usecs

#define ICEMU_SEND_PINLOW 0x0 << 5
#define ICEMU_SEND_PINHIGH 0x1 << 5
#define ICEMU_SEND_PININPUT 0x2 << 5
#define ICEMU_SEND_PINOUTPUT 0x3 << 5
#define ICEMU_SEND_ENDINTERRUPT 0x4 << 5

#define ICEMU_RECV_PINLOW 0x0 << 5
#define ICEMU_RECV_PINHIGH 0x1 << 5
#define ICEMU_RECV_TICK 0x2 << 5 // Time passes by, increment counters
#define ICEMU_RECV_INTERRUPT 0x3 << 5 // interrupt triggered

// 1 tick == ICEMU_TIME_RESOLUTION usecs
static unsigned long current_ticks = 0;
static unsigned long timers_resolution[MAX_TIMERS] = {0}; // in ticks. 0 = disabled
static unsigned long timers_counter[MAX_TIMERS] = {0}; // in ticks. 0 = threshold reached, time to set flag
static bool timers_flag[MAX_TIMERS] = {0}; // timer triggered
// There can ever only be one interrupt running at once. -1 means no interrupt
static char current_interrupt_id = -1;
static bool pins_high[MAX_PINS] = {0}; // Whether a pin is high

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
    assert(current_interrupt_id == -1);
    msg &= MAX_5BITS;
    current_interrupt_id = msg;
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
        case ICEMU_RECV_PINLOW:
            pins_high[msg & MAX_5BITS] = false;
            return true;
        case ICEMU_RECV_PINHIGH:
            pins_high[msg & MAX_5BITS] = true;
            return true;
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

    pins_high[pin & MAX_5BITS] = high;
    msg = high ? ICEMU_SEND_PINHIGH : ICEMU_SEND_PINLOW;
    putchar(msg | (pin & MAX_5BITS));
    fflush(stdout);
}

void icemu_pinsetmode(unsigned char pin, bool output)
{
    unsigned char r;
    unsigned char msg;

    msg = output ? ICEMU_SEND_PINOUTPUT : ICEMU_SEND_PININPUT;
    putchar(msg | (pin & MAX_5BITS));
    fflush(stdout);

    while (!output) {
        r = readchar();
        assert(process_message(r));
        msg = r & 0xe0;
        // waiting for a pin set...
        if (((msg == ICEMU_RECV_PINLOW) || (msg == ICEMU_RECV_PINHIGH)) && ((r & MAX_5BITS) == (pin & MAX_5BITS))) {
            break;
        }
    }
}

bool icemu_pinread(unsigned char pin)
{
    return pins_high[pin];
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

// Returns the ID of the current interrupt, or -1 if none.
// if an interrupt is returned, you have to call icemu_end_interrupt()
// as soon as the interrupt is finished.
char icemu_check_interrupt()
{
    return current_interrupt_id;
}

void icemu_end_interrupt()
{
    current_interrupt_id = -1;
    putchar(ICEMU_SEND_ENDINTERRUPT);
    fflush(stdout);
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
