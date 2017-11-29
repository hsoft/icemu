#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "ui.h"
#include "chip.h"
#include "sim.h"
#include "util.h"
#include "private.h"

#define MAX_ELEMENTS 100
#define REFRESH_UI_INTERVAL 200 * 1000

typedef struct {
    const char *title;
    const ICeChip *chip;
} Element;

static WINDOW *main_window;
static Element * elements[MAX_ELEMENTS] = { 0 };
static char * labels[MAX_ELEMENTS] = { 0 };
static time_t last_refresh_ts = 0;
static bool show_keybindings = false;

/* Private */
static void ui_refresh_elements()
{
    int x, y, acc_y, maxw, i, j, elemw, elemh;
    ICeChipAsciiArt asciiart;

    y = 0;
    x = 0;
    acc_y = 0;
    maxw = getmaxx(main_window);
    for (i = 0; i < MAX_ELEMENTS; i++) {
        if (elements[i] == NULL) {
            break;
        }
        elements[i]->chip->asciiart_func(elements[i]->chip, &asciiart);
        elemw = asciiart.width;
        if (elemw + x >= maxw) {
            x = 0;
            y += acc_y;
            acc_y = 0;
        }
        // +1 for title
        elemh = asciiart.height + 1;
        acc_y = MAX(acc_y, elemh);

        mvaddnstr(y, x, elements[i]->title, elemw - 1);
        for (j = 0; j < (elemh - 1); j++) {
            // -1 to remove the newline
            mvaddnstr(y+j+1, x, &asciiart.contents[(j * elemw)], elemw - 1);
        }

        x += elemw + 1;
    }
}

static void ui_draw_menu()
{
    int x, y, maxh, i, lblw, lblh;

    maxh = getmaxy(main_window);
    lblw = 0;
    for (i = 0; i < MAX_ELEMENTS; i++) {
        if (labels[i] == NULL) {
            break;
        }
        lblw = MAX(lblw, strnlen(labels[i], 0xff));
    }
    lblh = i;
    // -1 because we need one line for the bottom bar
    y = maxh - lblh - 1;
    x = 0;
    for (i = 0; i < lblh; i++) {
        mvaddnstr(y+i, x, labels[i], lblw);
    }
}

static void ui_draw_debug_values()
{
    Simulation *sim = icemu_sim_get();
    struct DebugValue *dv;
    int y, x, rightx, scount, i;
    char s[100];

    if (sim->debug_values[0].name == NULL) {
        return; // nothing to draw
    }

    y = getmaxy(main_window) - 2; // -2 because we don't draw over bottom bar
    rightx = getmaxx(main_window);
    for (i = 0; i < MAX_DEBUG_VALUES; i++) {
        dv = &sim->debug_values[i];
        if (dv->name == NULL) {
            break;
        }
        scount = sprintf(s, "%s: %d", dv->name, dv->val);
        x = rightx - scount;
        mvaddnstr(y, x, s, scount);
    }
}

static void ui_draw_bottom_bar()
{
    int maxy, count;
    time_t elapsed;
    unsigned int elapsed_s, elapsed_sub;
    char s[100];
    const char *paused_s;
    char slowdown_s[20];;

    elapsed = icemu_sim_elapsed_usecs();
    elapsed_s = elapsed / (1000 * 1000);
    elapsed_sub = (elapsed / (100 * 1000)) % 10;

    paused_s = icemu_sim_runmode() == ICE_SIM_RUNMODE_PAUSE ? " (Paused)" : "";
    if (icemu_sim_slowdown_factor() > 1) {
        sprintf(slowdown_s, " (Slowdown: %dX)", icemu_sim_slowdown_factor());
    } else {
        slowdown_s[0] = '\0';
    }
    count = sprintf(
        s, "(?) Keybindings  Time: %d.%ds%s%s Ticks: %ld",
        elapsed_s, elapsed_sub, slowdown_s, paused_s, icemu_sim_ticks());
    maxy = getmaxy(main_window);
    mvaddnstr(maxy - 1, 0, s, count);
}

static int ui_getkey()
{
    int ch;

    ch = getch();
    if (ch == '?') {
        show_keybindings = !show_keybindings;
        ch = -1;
    }
    return ch;
}

/* Public */
void icemu_ui_init()
{
    main_window = initscr();
    noecho();
    cbreak();
    curs_set(0);
    nodelay(main_window, true);
}

void icemu_ui_deinit()
{
    nocbreak();
    echo();
    endwin();
    curs_set(1);
}

void icemu_ui_add_element(char *title, const ICeChip *chip)
{
    uint8_t i;

    for (i = 0; i < MAX_ELEMENTS; i++) {
        if (elements[i] == NULL) {
            break;
        }
    }
    elements[i] = malloc(sizeof(Element));
    elements[i]->title = title;
    elements[i]->chip = chip;
}

void icemu_ui_add_label(char *name)
{
    uint8_t i;

    for (i = 0; i < MAX_ELEMENTS; i++) {
        if (labels[i] == NULL) {
            break;
        }
    }
    labels[i] = name;
}

// Returns the key that was pressed or -1 if none
int icemu_ui_refresh()
{
    time_t ts;

    ts = icemu_util_timestamp();
    if (ts - last_refresh_ts >= REFRESH_UI_INTERVAL) {
        last_refresh_ts = ts;
        erase();
        ui_refresh_elements();
        if (show_keybindings) {
            ui_draw_menu();
        }
        ui_draw_debug_values();
        ui_draw_bottom_bar();
        refresh();
    }
    return ui_getkey();
}
