#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct ICeChip ICeChip;
typedef struct ICePinList ICePinList;

#define MAX_PINS_ON_A_WIRE 128
#define MAX_CODE_LEN 10

/* About pins:
 *
 * A pin's goal is to be either high or low. When wired to other pins, that high or low property
 * propagates to other pins. When attached to a chip, it influences that chip's logic or is
 * influenced by it.
 *
 * Pins also have the concept of "enabled". Normally, a pin being enabled is the same thing as
 * being high, but some pins on some chips are inverted. If your pin code starts with `~`, then
 * it is such a pin and "enabled" will mean "low" on that pin.
 */

/* About wires:
 *
 * A pin can only ever be associated to one wire because whatever it's wired to
 * is connected to everything else touching the wire. Therefore, our pin starts unconnected and
 * if we connect to another unconnected wire, we create a new wire. if we connect to a connected
 * wire, we add ourselves to that wire. If we connect two pins with different wires, we merge the
 * two wires.
 */

/* About input/output:
 *
 * An output pin emits high or low signals and an input pin receives them. Unwired, this doesn't
 * make such a difference. When wired, however, the difference is fundamental. On a wire, only
 * input pins receive signal propagation. output pins are left untouched. Also, when deciding
 * whether the wire is high or low, only output pins are considered. As soon as one output pin is
 * high, the wire is high.
 */

/* About oscillation:
 *
 * Oscillation frequency is the property of a pin that goes high/low at a specific frequency. It's
 * an approximation whose goal is to save CPU cycles. For example, if you have a pin oscillating
 * at 1 MHz connected to a 16bit counter, why bother calculating the result of so many high/low
 * triggers? You already know beforehand what kind of output you're going to get from the counter.
 *
 * That's what oscillation is about. It propagates on a wire and through Chips that support
 * oscillation until it can't. The endgame of an oscillation propagation is either to display
 * frequencies in the UI or to generate interrupts on a MCU. Because yes, when a interrupt pin
 * oscillates on a MCU, a proper amount of interrupts are generated on each tick.
 *
 * An oscillating pin always evaluate to high.
 */

typedef struct ICePin {
    ICeChip *chip;
    const char *code;
    bool output;
    bool low_means_high;
    bool high;
    unsigned int oscillating_freq;
    ICePinList *wire;
} ICePin;

typedef void (ICePinChangeFunc)(ICePin *);

/* Allocates and initializes a new pin
 */
ICePin* icemu_pin_new(ICeChip *chip, const char *code, bool output);

/* Initializes `pin`
 *
 * chip: the chip it belongs to. Can be NULL.
 * code: the pin code on the chip. Used in asciiart.
 * output: whether the pin is an input or an output pin.
 */
void icemu_pin_init(ICePin *pin, ICeChip *chip, const char *code, bool output);

/* Sets whether `pin` is high
 *
 * returns true if a change occurred. false if the pin was already of the specified state.
 */
bool icemu_pin_set(ICePin *pin, bool high);

/* Sets whether `pin` is enabled
 */
bool icemu_pin_enable(ICePin *pin, bool enabled);

/* Whether `pin` is enabled
 */
bool icemu_pin_isenabled(const ICePin *pin);

/* Sets `pin`'s oscillating frequency
 */
void icemu_pin_set_oscillating_freq(ICePin *pin, unsigned int freq);

/* Wire `pin` to `other`
 *
 * If `pin` or `other` were already wired to other pins, all those pins are now on the same wire.
 */
void icemu_pin_wireto(ICePin *pin, ICePin *other);
