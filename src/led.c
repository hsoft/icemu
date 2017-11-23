#include <stdlib.h>

#include "led.h"

/* Private */
/*  _.  A.
 * |_| FGB
 * |_| EDC
 */
static void seg7_asciiart(Chip *chip, ChipAsciiArt *dst)
{
    char *s;
    Pin **pins;

    pins = chip->pins.pins;
    s = dst->contents;
    dst->width = 4;
    dst->height = 3;

    s[0] = ' '; // always blank;
    s[1] = !pins[0]->high ? '_' : ' '; // A
    s[2] = !pins[7]->high ? '.' : ' '; // DP
    s[3] = '\n';
    s[4] = !pins[5]->high ? '|' : ' '; // F
    s[5] = !pins[6]->high ? '_' : ' '; // G
    s[6] = !pins[1]->high ? '|' : ' '; // B
    s[7] = '\n';
    s[8] = !pins[4]->high ? '|' : ' '; // E
    s[9] = !pins[3]->high ? '_' : ' '; // D
    s[10] = !pins[2]->high ? '|' : ' '; // C
    s[11] = '\n';
    s[12] = '\0';
}

static void ledmatrix_asciiart(Chip *chip, ChipAsciiArt *dst)
{
    uint8_t i, offset;
    char *s;
    Pin **pins;
    LEDMatrix *lm;

    lm = (LEDMatrix *)chip->logical_unit;
    pins = chip->pins.pins;
    s = dst->contents;
    dst->width = lm->width + 1; // + 1 for \n
    dst->height = lm->height;

    for (i = 0; i < lm->height; i++) {
        s[i * dst->width] = '\n';
    }
    for (i = 0; i < (lm->width * lm->height); i++) {
        // for every line, we accumulate an offset to insert newlines
        offset = i / lm->width;
        s[i + offset] = !pins[i]->high ? 'X' : '.';
    }
    s[lm->width * lm->height] = '\0';
}

/* Public */
void icemu_seg7_init(Chip *chip)
{
    uint8_t i;
    const char * codes[] = {"A", "B", "C", "D", "E", "F", "G", "DP", NULL};

    icemu_chip_init(chip, NULL, NULL, 8);
    for (i = 0; i < 8; i++) {
        icemu_chip_addpin(chip, codes[i], false, false);
    }
    chip->asciiart_func = seg7_asciiart;
}

void icemu_ledmatrix_init(Chip *chip, uint8_t width, uint8_t height)
{
    LEDMatrix *lm;
    uint8_t i;

    lm = (LEDMatrix *)malloc(sizeof(LEDMatrix));
    icemu_chip_init(chip, (void *)lm, NULL, width * height);
    lm->width = width;
    lm->height = height;
    for (i = 0; i < width * height; i++) {
        icemu_chip_addpin(chip, "", false, false);
    }
    chip->asciiart_func = ledmatrix_asciiart;
}
