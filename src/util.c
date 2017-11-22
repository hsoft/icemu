#include <stdlib.h>

#include "util.h"

uint8_t icemu_util_chararray_count(char **array)
{
    uint8_t result = 0;

    while (array[result] != NULL) {
        result++;
    }
    return result;
}
