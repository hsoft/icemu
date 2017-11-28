#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct Chip Chip;
typedef struct ICePinList ICePinList;

#define MAX_PINS_ON_A_WIRE 128
#define MAX_CODE_LEN 10


// About wires:
// A pin can only ever be associated to one wire because whatever it's wired to
// is connected to everything else touching the wire. Therefore, our pin starts unconnected and
// if we connect to another unconnected wire, we create a new wire. if we connect to a connected
// wire, we add ourselves to that wire. If we connect two pins with different wires, we merge the
// two wires.

typedef struct Pin {
    Chip *chip;
    const char *code;
    bool output;
    bool low_means_high;
    bool high;
    unsigned int oscillating_freq;
    ICePinList *wire;
} Pin;

typedef void (*PinChangeFunc)(Pin *);

Pin* icemu_pin_new(Chip *chip, const char *code, bool output);
void icemu_pin_init(Pin *pin, Chip *chip, const char *code, bool output);
bool icemu_pin_check_if_changed(Pin *pin);
// returns true if a change occurred
bool icemu_pin_set(Pin *pin, bool high);
// same as set() if low_means_high is false, otherwise, the opposite.
bool icemu_pin_enable(Pin *pin, bool enabled);
bool icemu_pin_isenabled(Pin *pin);
void icemu_pin_set_oscillating_freq(Pin *pin, unsigned int freq);
void icemu_pin_wireto(Pin *pin, Pin *other);
void icemu_pin_set_global_pinchange_trigger(PinChangeFunc func);
