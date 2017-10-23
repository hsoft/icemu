#pragma once
#include <stdbool.h>

void icemu_pinset(unsigned char pin, bool high);
void icemu_pinsetmode(unsigned char pin, bool output);
bool icemu_pinread(unsigned char pin);
void icemu_delay_us(unsigned int us);
void icemu_delay_ms(unsigned int ms);
char icemu_check_interrupt();
void icemu_end_interrupt();
bool icemu_start_timer(unsigned char timer_id, unsigned long usecs);
bool icemu_check_timer(unsigned char timer_id);
void icemu_process_messages();
