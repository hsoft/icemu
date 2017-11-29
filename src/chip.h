#pragma once
#include <sys/time.h>
#include "pin.h"
#include "pinlist.h"

// maximum number of usecs we allow ourselves to wait in between elapse() calls
#define ICE_MAX_CHIP_ELAPSE 10000

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

/* Elapse functions are regularly called by the simulation for every chip that has one. The
 * `time_t` argument is the number of usecs that has elapsed since the last elapse() call. that
 * number is not always the same thing and depends on the behavior of other chips.
 *
 * The return value is the number of usecs that we need to wait for this particular chip to
 * have something meaningful to do (other than waiting). The simulation will always call
 * elapse before that number of usecs, but it might call it sooner if another chip does something
 * sooner.
 *
 * If you don't know how many usecs you need, return 0.
 */
typedef unsigned int (ICeElapseFunc)(ICeChip *, time_t);

struct ICeChip {
    void *logical_unit;
    ICePinChangeFunc *pin_change_func;
    void (*asciiart_func)(const ICeChip *, ICeChipAsciiArt *);
    ICeElapseFunc *elapse_func;
    ICePinList pins;
};

/* Returns the pin belonging to `chip` with specified `code`
 *
 * Returns `NULL` if there's no match.
 */
ICePin* icemu_chip_getpin(const ICeChip *chip, const char *code);
