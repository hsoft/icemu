#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "chip.h"
#include "pin.h"
#include "pinlist.h"
#include "mcu.h"
#include "util.h"
#include "private.h"

/* Private */
static ICeChip * chip_registry[MAX_SIM_CHIPS] = { 0 };

void chip_add_to_registry(ICeChip *chip)
{
    int i;

    for (i = 0; i < MAX_SIM_CHIPS; i++) {
        if (chip_registry[i] == NULL) {
            chip_registry[i] = chip;
            break;
        }
    }
}

static uint8_t chip_maxcodelen(const ICeChip *chip)
{
    uint8_t result = 0;
    uint8_t i, l;
    int charcount;
    char tmpfmt[10];
    ICePin *p;

    for (i = 0; i < chip->pins.count; i++) {
        p = chip->pins.pins[i];
        l = strnlen(p->code, MAX_CODE_LEN);
        if (p->oscillating_freq > 0) {
            // We need extra space for freq display
            charcount = icemu_util_fmthertz(tmpfmt, p->oscillating_freq);
            l += charcount + 1; // max space taken for freq display
        }
        if (l > result) {
            result = l;
        }
    }

    return result;
}

// spacer (name + 1) twice and a fixed '_______' (7 chars) and 1 char for \n
static uint8_t chip_asciiart_width(const ICeChip *chip)
{
    uint8_t max_pincode_len;

    max_pincode_len = chip_maxcodelen(chip);
    return (max_pincode_len + 1) * 2 + 7 + 1;
}

// header line + ceil(nb pins // 2)
static uint8_t chip_asciiart_height(const ICeChip *chip)
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

static char chip_asciiart_pstatus(ICePin *pin)
{
    if (pin->oscillating_freq > 0) {
        return '~';
    } else {
        return pin->high ? '+' : '-';
    }
}

static void chip_asciiart_add_freq_display(char *dst, ICePin *pin, bool rightalign)
{
    char tmpfmt[10];
    int charcount;

    if (pin->oscillating_freq == 0) {
        return;
    }
    *dst = '@';
    charcount = icemu_util_fmthertz(tmpfmt, pin->oscillating_freq);
    if (rightalign) {
        dst -= charcount;
    } else {
        dst++;
    }
    memcpy(dst, tmpfmt, charcount);
}

static void chip_asciiart(const ICeChip *chip, ICeChipAsciiArt *dst)
{
    uint8_t w, h, mcl, i, right_pin_start, slen;
    uint16_t loffset, codeoffset;
    ICePin *p;
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
        codeoffset = loffset + mcl - slen;
        memcpy(&s[codeoffset], p->code, slen);
        chip_asciiart_add_freq_display(&s[codeoffset - 1], p, true);
        s[loffset + mcl] = p->output ? '<' : '>';
        s[loffset + mcl + 1] = '|';
        s[loffset + mcl + 2] = chip_asciiart_pstatus(p);
        s[loffset + mcl + 7] = '|';
        if (i + right_pin_start < chip->pins.count) {
            p = chip->pins.pins[i + right_pin_start];
            s[loffset + mcl + 6] = chip_asciiart_pstatus(p);
            s[loffset + mcl + 8] = p->output ? '>' : '<';
            codeoffset = loffset + mcl + 9;
            slen = strnlen(p->code, mcl);
            strncpy(&s[codeoffset], p->code, slen);
            chip_asciiart_add_freq_display(&s[codeoffset + slen], p, false);
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

/* Protected */
ICeChip** icemu_chip_get_registry()
{
    return chip_registry;
}

/* Public */
void icemu_chip_init(
    ICeChip *chip, void *logical_unit, ICePinChangeFunc *pin_change_func, uint8_t pin_count)
{
    chip->logical_unit = logical_unit;
    chip->pin_change_func = pin_change_func;
    chip->asciiart_func = chip_asciiart;
    chip->elapse_func = NULL;
    icemu_pinlist_init(&chip->pins, pin_count);
    chip_add_to_registry(chip);
}

ICePin* icemu_chip_addpin(ICeChip *chip, const char *code, bool output)
{
    ICePin *result;

    result = icemu_pin_new(chip, code, output);
    icemu_pinlist_add(&chip->pins, result);
    return result;
}

void icemu_chip_addpins(ICeChip *chip, ICePinList *dst_pinlist, const char **codes, bool output)
{
    uint8_t count, i;

    count = icemu_util_chararray_count(codes);
    icemu_pinlist_init(dst_pinlist, count);
    for (i = 0; i < count; i++) {
        icemu_pinlist_add(dst_pinlist, icemu_chip_addpin(chip, codes[i], output));
    }
}

ICePin* icemu_chip_getpin(const ICeChip *chip, const char *code)
{
    return icemu_pinlist_find_by_code(&chip->pins, code);
}

unsigned int icemu_chip_elapse(ICeChip *chip, time_t usecs)
{
    if (chip->elapse_func != NULL) {
        return chip->elapse_func(chip, usecs);
    }
    return 0;
}
