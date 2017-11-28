#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct ICeChip ICeChip;
typedef struct ICePinList ICePinList;

#define MAX_PINS_ON_A_WIRE 128
#define MAX_CODE_LEN 10


// About wires:
// A pin can only ever be associated to one wire because whatever it's wired to
// is connected to everything else touching the wire. Therefore, our pin starts unconnected and
// if we connect to another unconnected wire, we create a new wire. if we connect to a connected
// wire, we add ourselves to that wire. If we connect two pins with different wires, we merge the
// two wires.

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

ICePin* icemu_pin_new(ICeChip *chip, const char *code, bool output);
void icemu_pin_init(ICePin *pin, ICeChip *chip, const char *code, bool output);
// returns true if a change occurred
bool icemu_pin_set(ICePin *pin, bool high);
// same as set() if low_means_high is false, otherwise, the opposite.
bool icemu_pin_enable(ICePin *pin, bool enabled);
bool icemu_pin_isenabled(const ICePin *pin);
void icemu_pin_set_oscillating_freq(ICePin *pin, unsigned int freq);
void icemu_pin_wireto(ICePin *pin, ICePin *other);
void icemu_pin_set_global_pinchange_trigger(ICePinChangeFunc *func);
