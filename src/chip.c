#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "chip.h"
#include "mcu.h"
#include "util.h"

#define MAX_CODE_LEN 10

/* Private */

static uint8_t chip_maxcodelen(Chip *chip)
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

// spacer (name + 1) twice and a fixed '_______' (7 chars) and 1 char for \n
static uint8_t chip_asciiart_width(Chip *chip)
{
    uint8_t max_pincode_len;

    max_pincode_len = chip_maxcodelen(chip);
    return (max_pincode_len + 1) * 2 + 7 + 1;
}

// header line + ceil(nb pins // 2)
static uint8_t chip_asciiart_height(Chip *chip)
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

static void chip_asciiart(Chip *chip, ChipAsciiArt *dst)
{
    uint8_t w, h, mcl, i, right_pin_start, slen;
    uint16_t loffset;
    Pin *p;
    char *s;

    s = dst->contents;
    w = chip_asciiart_width(chip);
    h = chip_asciiart_height(chip);
    mcl = chip_maxcodelen(chip);
    right_pin_start = h - 1;

    memset(s, ' ', w * h);
    memset(&s[mcl+1], '_', 7);
    s[w-1] = '\n';

    for (i = 0; i < (h - 1); i++) {
        loffset = (uint16_t)(w * (i + 1));
        p = chip->pins.pins[i];
        slen = strnlen(p->code, mcl);
        memcpy(&s[loffset], p->code, slen);
        s[loffset + mcl] = p->output ? '<' : '>';
        s[loffset + mcl + 1] = '|';
        s[loffset + mcl + 2] = p->high ? '+' : '-';
        s[loffset + mcl + 7] = '|';
        if (i + right_pin_start < chip->pins.count) {
            p = chip->pins.pins[i + right_pin_start];
            s[loffset + mcl + 6] = p->high ? '+' : '-';
            s[loffset + mcl + 8] = p->output ? '>' : '<';
            strncpy(&s[loffset + mcl + 9], p->code, mcl);
        } else {
            // close the rectangle in odd pins scenarios
            s[loffset + mcl + 6] = '_';
        }
        s[loffset + w - 1] = '\n';
    }
    memset(&s[(w * h) - mcl - 7], '_', 3);
    s[w * h] = '\0';
    dst->width = w;
    dst->height = h;
}

/* Public */
void icemu_chip_init(Chip *chip, void *logical_unit, PinChangeFunc pin_change_func, uint8_t pin_count)
{
    chip->logical_unit = logical_unit;
    chip->pin_change_func = pin_change_func;
    chip->asciiart_func = chip_asciiart;
    chip->elapse_func = NULL;
    icemu_pinlist_init(&chip->pins, pin_count);
}

Pin* icemu_chip_addpin(Chip *chip, const char *code, bool output, bool low_means_high)
{
    Pin *result;

    result = icemu_pin_new(chip, code, output);
    icemu_pinlist_add(&chip->pins, result);
    return result;
}

Pin* icemu_chip_getpin(Chip *chip, const char *code)
{
    uint8_t i;
    const char *chip_code;

    for (i = 0; i < chip->pins.count; i++) {
        chip_code = chip->pins.pins[i]->code;
        if (chip_code[0] == '~') {
            chip_code++;
        }
        if (strncmp(code, chip_code, MAX_CODE_LEN) == 0) {
            return chip->pins.pins[i];
        }
    }
    return NULL;
}

void icemu_chip_elapse(Chip *chip, time_t usecs)
{
    if (chip->elapse_func != NULL) {
        chip->elapse_func(chip, usecs);
    }
}
