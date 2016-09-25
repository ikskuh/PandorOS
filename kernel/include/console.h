#pragma once

#include <stdint.h>

#define CHA_DEFAULT   0
#define CHA_HIGHLIGHT (1<<0)
#define CHA_RED       (1<<1)

#define MENU_DEFAULT    0
#define MENU_SELECTED   CHA_HIGHLIGHT
#define MENU_RED        CHA_RED
#define MENU_RIGHTALIGN (1<<2)

/**
 * An attributes character.
 */
typedef struct 
{
	char c;
	uint8_t attribs;
} attrchar_t;

typedef struct console
{
	const int width;
	const int height;
	struct { int x, y; } cursor;
	attrchar_t * const data;
} console_t;

typedef struct menuitem
{
	char label[64];
	int flags;
} menuitem_t;

typedef struct menu
{
	int length;
	menuitem_t *items;
} menu_t;

/**
 * The default console.
 */
extern console_t *stdcon;

/**
 * Initializes the console system.
 */
void console_init();

/**
 * Renders the given menu.
 * @param menu The menu to be rendered. If NULL, an empty menu will be drawn.
 */
void console_menu(menu_t const * menu);

/**
 * Sets the current stdcon.
 * @remarks If con is NULL, nothing will happen.
 */
void console_set(console_t *con);

/**
 * Creates a new console.
 */
console_t *console_new();

/**
 * Destroys a console.
 */
void console_delete(console_t *con);

/**
 * Clears the console
 */ 
void console_clear(console_t *con);

/**
 * Scrolls the console a number of lines up.
 */
void console_scroll(console_t *con, int lines);

/**
 * Sets a character on the console.
 */
void console_setc(console_t *con, int x, int y, char c);

/**
 * Sets the cursor position of the console.
 */
void console_setcursor(console_t *con, int x, int y);

/**
 * Puts a character to the console.
 */
void console_putc(console_t *con, char c);

/**
 * Puts a string to the console.
 */
void console_puts(console_t *con, char const * str);

/**
 * Puts a formatted string to the console.
 */
void console_printf(console_t *con, char const * fmt, ...);

/**
 * Clears the current console.
 */
void cls();

/**
 * Prints a character.
 */
void putc(char c);

/**
 * Prints a string.
 */
void puts(char const * str);

/**
 * Sets a character on the current console.
 */
void setc(int x, int y, char c);

/**
 * Sets the current cursor position.
 */ 
void setcursor(int x, int y);

/**
 * Prints a formatted string.
 */
void printf(char const * fmt, ...);
