#pragma once
#include <sys/time.h>
#include "pin.h"
#include "pinlist.h"

/* About chips:
 *
 * A chip is a generic integrated circuit. The logic in this unit is the logic that is common to
 * all ICs, which is mostly about holding pins together and being represented as asciiart.
 *
 * Each pin has a `logical_unit` pointer that points to a more specific structure (counter,
 * decoder, shiftregister, etc.). It's this unt that contains the juicy stuff.
 *
 * You don't create generic chips yourself. You create chips through the initializer of their
 * specific incarnation.
 */

typedef struct {
    uint16_t width;
    uint16_t height;
    char contents[4096]; // should be enough for any chip
} ICeChipAsciiArt;

struct ICeChip {
    void *logical_unit;
    ICePinChangeFunc *pin_change_func;
    void (*asciiart_func)(const ICeChip *, ICeChipAsciiArt *);
    void (*elapse_func)(ICeChip *, time_t);
    ICePinList pins;
};

/* Returns the pin belonging to `chip` with specified `code`
 *
 * Returns `NULL` if there's no match.
 */
ICePin* icemu_chip_getpin(const ICeChip *chip, const char *code);
