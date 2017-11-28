#pragma once
#include <stdint.h>
#include <sys/time.h>

#include "pin.h"

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

uint8_t icemu_util_chararray_count(const char **array);
void icemu_util_set_binary_value(ICePinList *pinlist, uint16_t val);
uint16_t icemu_util_get_binary_value(const ICePinList *pinlist);
time_t icemu_util_timestamp();
int icemu_util_fmthertz(char *dst, uint64_t hertz);
