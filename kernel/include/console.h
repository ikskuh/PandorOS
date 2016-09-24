#pragma once

#define MENU_DEFAULT  0
#define MENU_SELECTED (1<<0)
#define MENU_RED (1<<1)
#define MENU_RIGHTALIGN (1<<2)

typedef struct menu_entry
{
	char label[64];
	int flags;
} menu_entry_t;

/**
 * Initializes the text console.
 */
void console_init();

/**
 * Refreshes the console and rerenders the menu.
 */ 
void console_refresh();

/**
 * Moves the cursor if possible.
 */
void console_move(int x, int y);

/**
 * Sets the menu that is currently displayed.
 * @param entries A persistent array of menu entries. Is referenced, not copied!
 * @param count   The number of elements.
 */
void console_setmenu(menu_entry_t const * entries, int count);

/**
 * Clears the screen.
 */
void cls();

/**
 * Puts a string to the console.
 */
void puts(char const *string);

/**
 * Puts a character on the screen.
 */
void putc(char c);

/**
 * Classic printf function for debugging.
 */ 
int printf(char const *fmt, ...);