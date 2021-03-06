#include <stdlib.h>

#include "led.h"
#include "util.h"
#include "private.h"

// usecs
#define LED_FADE_DELAY 10000

/* Private */
typedef struct {
    uint32_t fade_timeout;
    ICePin *vcc;
    ICePin *gnd;
} LED;

typedef struct {
    uint8_t width;
    uint8_t height;
    ICePin common;
    LED *leds;
} LEDMatrix;

void led_init(LED *led, ICePin *vcc, ICePin *gnd)
{
    led->fade_timeout = 0;
    led->vcc = vcc;
    led->gnd = gnd;
}

bool led_powered(LED *led)
{
    return led->vcc->high && !led->gnd->high;
}

unsigned int led_elapse(LED *led, time_t usecs)
{
    if (led_powered(led)) {
        led->fade_timeout = LED_FADE_DELAY;
        return 0;
    }
    if (led->fade_timeout > 0) {
        led->fade_timeout -= MIN(usecs, led->fade_timeout);
    }
    return led->fade_timeout;
}

bool led_lit(LED *led)
{
    return led_powered(led) || led->fade_timeout > 0;
}

/*  _.  A.
 * |_| FGB
 * |_| EDC
 */
static void seg7_asciiart(const ICeChip *chip, ICeChipAsciiArt *dst)
{
    char *s;
    LEDMatrix *lm;

    lm = (LEDMatrix *)chip->logical_unit;
    s = dst->contents;
    dst->width = 4;
    dst->height = 3;

    s[0] = ' '; // always blank;
    s[1] = led_lit(&lm->leds[0]) ? '_' : ' '; // A
    s[2] = led_lit(&lm->leds[7]) ? '.' : ' '; // DP
    s[3] = '\n';
    s[4] = led_lit(&lm->leds[5]) ? '|' : ' '; // F
    s[5] = led_lit(&lm->leds[6]) ? '_' : ' '; // G
    s[6] = led_lit(&lm->leds[1]) ? '|' : ' '; // B
    s[7] = '\n';
    s[8] = led_lit(&lm->leds[4]) ? '|' : ' '; // E
    s[9] = led_lit(&lm->leds[3]) ? '_' : ' '; // D
    s[10] = led_lit(&lm->leds[2]) ? '|' : ' '; // C
    s[11] = '\n';
    s[12] = '\0';
}

static void ledmatrix_asciiart(const ICeChip *chip, ICeChipAsciiArt *dst)
{
    uint8_t i, offset;
    char *s;
    LEDMatrix *lm;

    lm = (LEDMatrix *)chip->logical_unit;
    s = dst->contents;
    dst->width = lm->width + 1; // + 1 for \n
    dst->height = lm->height;

    for (i = 0; i < lm->height; i++) {
        s[i * dst->width] = '\n';
    }
    for (i = 0; i < (lm->width * lm->height); i++) {
        // for every line, we accumulate an offset to insert newlines
        offset = i / lm->width;
        s[i + offset] = led_lit(&lm->leds[i]) ? 'X' : '.';
    }
    s[lm->width * lm->height] = '\0';
}

static unsigned int ledmatrix_elapse(ICeChip *chip, time_t usecs)
{
    int i;
    LEDMatrix *lm;
    unsigned int result = 0;
    unsigned int next_elapse;

    lm = (LEDMatrix *)chip->logical_unit;
    for (i = 0; i < lm->width * lm->height; i++) {
        next_elapse = led_elapse(&lm->leds[i], usecs);
        if (next_elapse > 0) {
            if (result > 0) {
                result = MIN(result, next_elapse);
            } else {
                result = next_elapse;
            }
        }
    }
    return result;
}

/* Public */
void icemu_seg7_init(ICeChip *chip)
{
    uint8_t i;
    const char * codes[] = {"A", "B", "C", "D", "E", "F", "G", "DP", NULL};

    icemu_ledmatrix_init(chip, 8, 1, true);
    for (i = 0; i < 8; i++) {
        chip->pins.pins[i]->code = codes[i];
    }
    chip->asciiart_func = seg7_asciiart;
}

void icemu_ledmatrix_init(
    ICeChip *chip, uint8_t width, uint8_t height, bool common_anode)
{
    LEDMatrix *lm;
    uint8_t i;
    ICePin *p;

    lm = (LEDMatrix *)malloc(sizeof(LEDMatrix));
    icemu_chip_init(chip, (void *)lm, NULL, width * height);
    lm->width = width;
    lm->height = height;
    lm->leds = malloc(sizeof(LED) * width * height);
    for (i = 0; i < width * height; i++) {
        p = icemu_chip_addpin(chip, "", false);
        /* We initially set our LED GND pin to the same value as the common pin
         * so that it doesn't start off as "powered" until we actually wire GND
         * to something low.
         */
        p->high = common_anode;
        if (common_anode) {
            led_init(&lm->leds[i], &lm->common, p);
        } else {
            led_init(&lm->leds[i], p, &lm->common);
        }
    }
    icemu_pin_init(&lm->common, chip, common_anode ? "CA" : "CC", false);
    chip->asciiart_func = ledmatrix_asciiart;
    chip->elapse_func = ledmatrix_elapse;
    icemu_pin_set(&lm->common, common_anode);
}

ICePin* icemu_ledmatrix_common_pin(ICeChip *chip)
{
    return &((LEDMatrix *)chip->logical_unit)->common;
}
