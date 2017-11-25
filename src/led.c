#include <stdlib.h>

#include "led.h"
#include "util.h"

// usecs
#define LED_FADE_DELAY 10000

/* Private */
void led_init(LED *led, Pin *vcc, Pin *gnd)
{
    led->fade_timeout = 0;
    led->powered = false;
    led->vcc = vcc;
    led->gnd = gnd;
}

void led_update(LED *led)
{
    led->powered = led->vcc->high && !led->gnd->high;
    if (!led->powered) {
        // enable fade timeout
        led->fade_timeout = LED_FADE_DELAY;
    }
}

void led_elapse(LED *led, time_t usecs)
{
    if (led->fade_timeout > 0) {
        led->fade_timeout -= MIN(usecs, led->fade_timeout);
    }
}

bool led_lit(LED *led)
{
    return led->powered || led->fade_timeout > 0;
}

static void ledmatrix_pinchange(Pin *pin)
{
    LEDMatrix *lm = (LEDMatrix *)pin->chip->logical_unit;
    int pinindex;

    pinindex = icemu_pinlist_find(&pin->chip->pins, pin);
    if (pinindex >=0) {
        led_update(&lm->leds[pinindex]);
    }
}

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

static void ledmatrix_elapse(Chip *chip, time_t usecs)
{
    int i;
    LEDMatrix *lm;

    lm = (LEDMatrix *)chip->logical_unit;
    for (i = 0; i < lm->width * lm->height; i++) {
        led_elapse(&lm->leds[i], usecs);
    }
}

/* Public */
void icemu_seg7_init(Chip *chip)
{
    uint8_t i;
    const char * codes[] = {"A", "B", "C", "D", "E", "F", "G", "DP", NULL};

    icemu_ledmatrix_init(chip, 8, 1);
    for (i = 0; i < 8; i++) {
        chip->pins.pins[i]->code = codes[i];
    }
    chip->asciiart_func = seg7_asciiart;
}

void icemu_ledmatrix_init(Chip *chip, uint8_t width, uint8_t height)
{
    LEDMatrix *lm;
    uint8_t i;

    lm = (LEDMatrix *)malloc(sizeof(LEDMatrix));
    icemu_chip_init(chip, (void *)lm, ledmatrix_pinchange, width * height);
    lm->width = width;
    lm->height = height;
    icemu_pin_init(&lm->vcc, chip, "VCC", false);
    lm->leds = malloc(sizeof(LED) * width * height);
    for (i = 0; i < width * height; i++) {
        led_init(&lm->leds[i], &lm->vcc, icemu_chip_addpin(chip, "", false, false));
    }
    chip->asciiart_func = ledmatrix_asciiart;
    chip->elapse_func = ledmatrix_elapse;
}

Pin* icemu_ledmatrix_vcc(Chip *chip)
{
    return &((LEDMatrix *)chip->logical_unit)->vcc;
}
