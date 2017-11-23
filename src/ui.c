#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "ui.h"
#include "chip.h"
#include "util.h"

#define MAX_ELEMENTS 100

typedef struct {
    char *title;
    Chip *chip;
} Element;

static WINDOW *main_window;
static Element * elements[MAX_ELEMENTS] = { 0 };
static char * labels[MAX_ELEMENTS] = { 0 };

/* Private */
static void ui_refresh_elements()
{
    int x, y, acc_y, maxh, maxw, i, j, elemw, elemh;
    char asciiart[4096];

    y = 0;
    x = 0;
    acc_y = 0;
    getmaxyx(main_window, maxh, maxw);
    for (i = 0; i < MAX_ELEMENTS; i++) {
        if (elements[i] == NULL) {
            break;
        }
        elemw = icemu_chip_asciiart_width(elements[i]->chip);
        if (elemw + x >= maxw) {
            x = 0;
            y += acc_y;
            acc_y = 0;
        }
        // +1 for title
        elemh = icemu_chip_asciiart_height(elements[i]->chip) + 1;
        acc_y = MAX(acc_y, elemh);

        icemu_chip_asciiart(elements[i]->chip, asciiart);
        for (j = 0; j < elemh; j++) {
            // -1 to remove the newline
            mvaddnstr(y+j, x, &asciiart[(j * elemw)], elemw - 1);
        }
    }
}

static void ui_refresh_labels()
{
    int x, y, maxh, maxw, i, lblw, lblh;

    getmaxyx(main_window, maxh, maxw);
    lblw = 0;
    for (i = 0; i < MAX_ELEMENTS; i++) {
        if (labels[i] == NULL) {
            break;
        }
        lblw = MAX(lblw, strnlen(labels[i], 0xff));
    }
    lblh = i;
    y = maxh - lblh - 1;
    x = 0;
    for (i = 0; i < lblh; i++) {
        mvaddnstr(y+i, x, labels[i], lblw);
    }
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

void icemu_ui_add_element(char *title, Chip *chip)
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
    erase();
    ui_refresh_elements();
    ui_refresh_labels();
    refresh();
    return getch();
}
