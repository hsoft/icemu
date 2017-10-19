#pragma once
#include <stdbool.h>

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

#define ICEMU_TIME_RESOLUTION 50 // in usecs

#define ICEMU_SEND_PINLOW 0x0 << 5
#define ICEMU_SEND_PINHIGH 0x1 << 5
#define ICEMU_SEND_PINREAD 0x2 << 5

#define ICEMU_RECV_PINLOW 0x0 << 5
#define ICEMU_RECV_PINHIGH 0x1 << 5
#define ICEMU_RECV_TICK 0x2 << 5 // Time passes by, increment counters

void icemu_pinset(unsigned char pin, bool high);
bool icemu_pinread(unsigned char pin);
void icemu_delay_us(unsigned int us);
void icemu_delay_ms(unsigned int ms);
bool icemu_start_timer(unsigned char timer_id, unsigned long usecs);
bool icemu_check_timer(unsigned char timer_id);
void icemu_tick_time();
