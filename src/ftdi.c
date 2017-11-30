#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <ftdi.h>

#include "ftdi.h"
#include "chip.h"
#include "util.h"
#include "private.h"

/* Private */
struct ftdi_context *g_ftdi = NULL;

static void ftdi_global_init()
{
    if (g_ftdi == NULL) {
        g_ftdi = ftdi_new();
        if (g_ftdi == NULL) {
            fprintf(stderr, "Couldn't initialize libftdi.\n");
            exit(1);
        }
    }
}

static void ftdi_pinchange(ICePin *pin)
{
}

// "_" suffix is because "ftdi_new()" clashes with libftdi.
static ICeFTDI* ftdi_new_(
    ICeChip *chip,
    uint32_t usb_vendor_id,
    uint32_t usb_product_id,
    const char **input_codes)
{
    ICeFTDI *f;

    ftdi_global_init();
    f = (ICeFTDI *)malloc(sizeof(ICeFTDI));
    f->handle = ftdi_usb_open(g_ftdi, usb_vendor_id, usb_product_id);
    if (f->handle < 0) {
        fprintf(
            stderr, "unable to open ftdi device: %d (%s)\n",
            f->handle, ftdi_get_error_string(g_ftdi));
        exit(1);
    }
    ftdi_set_bitmode(g_ftdi, 0xff, BITMODE_BITBANG);
    icemu_chip_init(chip, (void *)f, ftdi_pinchange, icemu_util_chararray_count(input_codes));
    icemu_chip_addpins(chip, &f->inputs, input_codes, false);
    return f;
}

/* Public */
void icemu_FT232H_init(ICeChip *chip)
{
    const char * input_codes[] = {"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", NULL};

    ftdi_new_(chip, 0x0403, 0x6014, input_codes);
}


