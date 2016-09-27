#pragma once

#include "console.h"
#include <stdbool.h>

#define MENU_DEFAULT    0
#define MENU_SELECTED   CHA_HIGHLIGHT
#define MENU_RED        CHA_RED
#define MENU_RIGHTALIGN (1<<2)
#define MENU_OPENED     (1<<3)

struct menuitem;

typedef bool (*menucallback_f)(struct menuitem *);

typedef struct menuitem
{
	char label[60];
	int flags;
	menucallback_f callback;
	int length;
	struct menuitem *items;
} menuitem_t;

typedef struct menu
{
	int length;
	struct menuitem *items;
} menu_t;

/**
 * Opens the menu and does input processing.
 * @param menu The menu to be opened.
 * @return     Returns the selected menu item.
 */
menuitem_t *menu_open(menu_t const * menu);

/**
 * Renders the given menu bar.
 */
void menu_render(menu_t const * menu);