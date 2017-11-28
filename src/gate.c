#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "gate.h"
#include "chip.h"
#include "util.h"

/* Private */
static bool test_nor(PinList *pinlist)
{
    int i;
    for (i = 0; i < pinlist->count; i++) {
        if (pinlist->pins[i]->high) {
            return false;
        }
    }
    return true;
}

static bool test_invert(PinList *pinlist)
{
    return !pinlist->pins[0]->high;
}

static bool gate_haspin(const Gate *gate, const Pin *pin)
{
    return icemu_pinlist_find(&gate->inputs, pin) >= 0 || pin == gate->output;
}

static Gate* gateset_find_pin(GateSet *gateset, const Pin *pin)
{
    Gate **g = gateset->gates;
    while (*g != NULL) {
        if (gate_haspin(*g, pin)) {
            return *g;
        }
        g++;
    }
    return NULL;
}

static void gate_pinchange(Pin *pin)
{
    Gate *g = gateset_find_pin((GateSet *)pin->chip->logical_unit, pin);
    icemu_pin_set(g->output, g->test_func(&g->inputs));
}

static Gate* gate_new(
    Chip *chip,
    GateTestFunc test_func,
    const char **input_codes,
    const char *output_code)
{
    Gate *g;

    g = (Gate *)malloc(sizeof(Gate));
    g->test_func = test_func;
    icemu_chip_addpins(chip, &g->inputs, input_codes, false);
    g->output = icemu_chip_addpin(chip, output_code, true);
    g->output->high = test_func(&g->inputs);
    return g;
}

static GateSet* gateset_new(Chip *chip, int gate_count, int pin_count)
{
    GateSet *gs;

    gs = (GateSet *)malloc(sizeof(GateSet));
    memset(gs->gates, 0, sizeof(gs->gates));
    icemu_chip_init(chip, (void *)gs, gate_pinchange, pin_count);
    return gs;
}

static void gateset_add_gate(GateSet *gateset, Gate *gate)
{
    Gate **g;

    g = gateset->gates;
    while (*g != NULL) g++;
    *g = gate;
}

/* Public */
/* NOR */
void icemu_CD4001B_init(Chip *chip)
{
    GateSet *gs;
    int i;
    const char * input_codes[4][3] = {
        {"A", "B", NULL},
        {"C", "D", NULL},
        {"E", "F", NULL},
        {"G", "H", NULL},
    };
    const char * output_codes[4] = {"J", "K", "L", "M"};

    gs = gateset_new(chip, 4, 4 * 3);
    for (i = 0; i < 4; i++) {
        gateset_add_gate(gs,
            gate_new(chip, test_nor, input_codes[i], output_codes[i]));
    }
}

void icemu_CD4002B_init(Chip *chip)
{
    GateSet *gs;
    int i;
    const char * input_codes[2][5] = {
        {"A", "B", "C", "D", NULL},
        {"E", "F", "G", "H", NULL},
    };
    const char * output_codes[2] = {"J", "K"};

    gs = gateset_new(chip, 2, 2 * 5);
    for (i = 0; i < 2; i++) {
        gateset_add_gate(gs,
            gate_new(chip, test_nor, input_codes[i], output_codes[i]));
    }
}

void icemu_CD4025B_init(Chip *chip)
{
    GateSet *gs;
    int i;
    const char * input_codes[3][4] = {
        {"A", "B", "C", NULL},
        {"D", "E", "F", NULL},
        {"G", "H", "I", NULL},
    };
    const char * output_codes[3] = {"J", "K", "L"};

    gs = gateset_new(chip, 3, 3 * 4);
    for (i = 0; i < 3; i++) {
        gateset_add_gate(gs,
            gate_new(chip, test_nor, input_codes[i], output_codes[i]));
    }
}

void icemu_SN74LS02_init(Chip *chip)
{
    GateSet *gs;
    int i;
    const char * input_codes[4][3] = {
        {"A1", "B1", NULL},
        {"A2", "B2", NULL},
        {"A3", "B3", NULL},
        {"A4", "B4", NULL},
    };
    const char * output_codes[4] = {"Y1", "Y2", "Y3", "Y4"};

    gs = gateset_new(chip, 4, 4 * 3);
    for (i = 0; i < 4; i++) {
        gateset_add_gate(gs,
            gate_new(chip, test_nor, input_codes[i], output_codes[i]));
    }
}

void icemu_SN74LS27_init(Chip *chip)
{
    GateSet *gs;
    int i;
    const char * input_codes[3][4] = {
        {"A1", "B1", "C1", NULL},
        {"A2", "B2", "C2", NULL},
        {"A3", "B3", "C3", NULL},
    };
    const char * output_codes[3] = {"Y1", "Y2", "Y3"};

    gs = gateset_new(chip, 3, 3 * 4);
    for (i = 0; i < 3; i++) {
        gateset_add_gate(gs,
            gate_new(chip, test_nor, input_codes[i], output_codes[i]));
    }
}

void icemu_SN54ALS27A_init(Chip *chip) { icemu_SN74LS27_init(chip); }
void icemu_SN54AAS27_init(Chip *chip) { icemu_SN74LS27_init(chip); }
void icemu_SN5427_init(Chip *chip) { icemu_SN74LS27_init(chip); }
void icemu_SN7427_init(Chip *chip) { icemu_SN74LS27_init(chip); }
void icemu_SN54LS27_init(Chip *chip) { icemu_SN74LS27_init(chip); }
void icemu_SN74ALS27A_init(Chip *chip) { icemu_SN74LS27_init(chip); }
void icemu_SN74AS27_init(Chip *chip) { icemu_SN74LS27_init(chip); }

void icemu_SN54S260_init(Chip *chip)
{
    GateSet *gs;
    int i;
    const char * input_codes[2][6] = {
        {"A1", "B1", "C1", "D1", "E1", NULL},
        {"A2", "B2", "C2", "D2", "E2", NULL},
    };
    const char * output_codes[2] = {"Y1", "Y2"};

    gs = gateset_new(chip, 2, 2 * 6);
    for (i = 0; i < 2; i++) {
        gateset_add_gate(gs,
            gate_new(chip, test_nor, input_codes[i], output_codes[i]));
    }
}

void icemu_SN74S260_init(Chip *chip) { icemu_SN54S260_init(chip); }
void icemu_SN74F260_init(Chip *chip) { icemu_SN54S260_init(chip); }

/* Inverter */
void icemu_SN74HC14_init(Chip *chip)
{
    GateSet *gs;
    int i;
    const char * input_codes[6][2] = {
        {"1A", NULL},
        {"2A", NULL},
        {"3A", NULL},
        {"4A", NULL},
        {"5A", NULL},
        {"6A", NULL},
    };
    const char * output_codes[6] = {"1Y", "2Y", "3Y", "4Y", "5Y", "6Y"};

    gs = gateset_new(chip, 6, 6 * 2);
    for (i = 0; i < 6; i++) {
        gateset_add_gate(gs,
            gate_new(chip, test_invert, input_codes[i], output_codes[i]));
    }
}
