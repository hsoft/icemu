#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct Chip Chip;
typedef struct PinList PinList;

#define MAX_PINS_ON_A_WIRE 128

// About wires:
// A pin can only ever be associated to one wire because whatever it's wired to
// is connected to everything else touching the wire. Therefore, our pin starts unconnected and
// if we connect to another unconnected wire, we create a new wire. if we connect to a connected
// wire, we add ourselves to that wire. If we connect two pins with different wires, we merge the
// two wires.

typedef struct Pin {
    Chip *chip;
    char *code;
    bool output;
    bool low_means_high;
    bool high;
    PinList *wire;
} Pin;

struct PinList {
    uint8_t capacity;
    uint8_t count;
    Pin **pins;
};

Pin* icemu_pin_new(Chip *chip, char *code, bool output, bool low_means_high);
void icemu_pin_init(Pin *pin, Chip *chip, char *code, bool output, bool low_means_high);
bool icemu_pin_check_if_changed(Pin *pin);
bool icemu_pin_set(Pin *pin, bool high); // returns true if a change occurred
void icemu_pin_wireto(Pin *pin, Pin *other);

PinList* icemu_pinlist_new(uint8_t capacity);
void icemu_pinlist_init(PinList *pinlist, uint8_t capacity);
void icemu_pinlist_destroy(PinList *pinlist);
void icemu_pinlist_add(PinList *pinlist, Pin *pin);
