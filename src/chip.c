#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "chip.h"
#include "util.h"

#define MAX_CODE_LEN 10

/* Private */

uint8_t chip_maxcodelen(Chip *chip)
{
    uint8_t result = 0;
    uint8_t i, l;

    for (i = 0; i < chip->pins.count; i++) {
        l = strnlen(chip->pins.pins[i]->code, MAX_CODE_LEN);
        if (l > result) {
            result = l;
        }
    }

    return result;
}

/* Public */
void icemu_chip_init(Chip *chip, void *logical_unit, PinChangeFunc pin_change_func, uint8_t pin_count)
{
    chip->logical_unit = logical_unit;
    chip->pin_change_func = pin_change_func;
    icemu_pinlist_init(&chip->pins, pin_count);
}

Pin* icemu_chip_addpin(Chip *chip, const char *code, bool output, bool low_means_high)
{
    Pin *result;

    result = icemu_pin_new(chip, code, output, low_means_high);
    icemu_pinlist_add(&chip->pins, result);
    return result;
}

Pin* icemu_chip_getpin(Chip *chip, char *code)
{
    uint8_t i;

    for (i = 0; i < chip->pins.count; i++) {
        if (strncmp(code, chip->pins.pins[i]->code, MAX_CODE_LEN) == 0) {
            return chip->pins.pins[i];
        }
    }
    return NULL;
}

// spacer (name + 1) twice and a fixed '_______' (7 chars) and 1 char for \n
uint8_t icemu_chip_asciiart_width(Chip *chip)
{
    uint8_t max_pincode_len;

    max_pincode_len = chip_maxcodelen(chip);
    return (max_pincode_len + 1) * 2 + 7 + 1;
}

// header line + ceil(nb pins // 2)
uint8_t icemu_chip_asciiart_height(Chip *chip)
{
    uint8_t pincount;
    uint8_t linecount;

    pincount = chip->pins.count;
    linecount = pincount / 2 + 1;
    if (pincount % 2 > 0) {
        // odd number of pins, we need an extra line
        linecount++;
    }
    return linecount;
}

uint16_t icemu_chip_asciiart_len(Chip *chip)
{
    uint8_t w, h;

    w = icemu_chip_asciiart_width(chip);
    h = icemu_chip_asciiart_height(chip);

    return (uint16_t)(w * h);
}

// dst *must* be big enough to accomodate icemu_chip_asciiart_len() + 1 (for \0)
void icemu_chip_asciiart(Chip *chip, char *dst)
{
    uint8_t w, h, mcl, i, right_pin_start;
    uint16_t loffset;
    Pin *p;

    w = icemu_chip_asciiart_width(chip);
    h = icemu_chip_asciiart_height(chip);
    mcl = chip_maxcodelen(chip);
    right_pin_start = h - 1;

    memset(dst, ' ', w * h);
    memset(&dst[mcl+1], '_', 7);
    dst[w-1] = '\n';

    for (i = 0; i < (h - 1); i++) {
        loffset = (uint16_t)(w * (i + 1));
        p = chip->pins.pins[i];
        strncpy(&dst[loffset], p->code, mcl);
        dst[loffset + mcl] = p->output ? '<' : '>';
        dst[loffset + mcl + 1] = '|';
        dst[loffset + mcl + 2] = p->high ? '+' : '-';
        dst[loffset + mcl + 7] = '|';
        if (i + right_pin_start < chip->pins.count) {
            p = chip->pins.pins[i + right_pin_start];
            dst[loffset + mcl + 6] = p->high ? '+' : '-';
            dst[loffset + mcl + 8] = p->output ? '>' : '<';
            strncpy(&dst[loffset + mcl + 9], p->code, mcl);
        }
        dst[loffset + w - 1] = '\n';
    }
    memset(&dst[(w * h) - mcl - 7], '_', 3);
    dst[w * h] = '\0';
}
