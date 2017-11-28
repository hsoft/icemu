#include <stdlib.h>
#include <stdio.h>

#include "util.h"

uint8_t icemu_util_chararray_count(const char **array)
{
    uint8_t result = 0;

    while (array[result] != NULL) {
        result++;
    }
    return result;
}

void icemu_util_set_binary_value(PinList *pinlist, uint16_t val)
{
    uint8_t i;

    for (i = 0; i < pinlist->count; i++) {
        icemu_pin_set(pinlist->pins[i], (val >> i) & 0x1);
    }
}

uint16_t icemu_util_get_binary_value(const PinList *pinlist)
{
    uint8_t i;
    uint8_t val = 0;

    for (i = 0; i < pinlist->count; i++) {
        if (pinlist->pins[i]->high) {
            val |= 1 << i;
        }
    }
    return val;
}

time_t icemu_util_timestamp()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 * 1000) + tv.tv_usec;
}

int icemu_util_fmthertz(char *dst, uint64_t hertz)
{
    const char * suffixes[] = {"Hz", "kHz", "MHz", "THz"};
    int order = 0;

    while ((order < 4) && (hertz / 1000 > 0)) {
        hertz /= 1000;
        order++;
    }
    return sprintf(dst, "%d%s", (int)hertz, suffixes[order]);
}
