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

#define ICEMU_SEND_PINLOW 0x0 << 5
#define ICEMU_SEND_PINHIGH 0x1 << 5
#define ICEMU_SEND_PINREAD 0x2 << 5

#define ICEMU_RECV_PINLOW 0x0 << 5
#define ICEMU_RECV_PINHIGH 0x1 << 5

void icemu_pinlow(unsigned char pin);
void icemu_pinhigh(unsigned char pin);
bool icemu_pinread(unsigned char pin);
void icemu_delay_us(unsigned int us);
void icemu_delay_ms(unsigned int ms);
