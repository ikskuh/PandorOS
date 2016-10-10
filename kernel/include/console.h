#pragma once

#include <stdint.h>

#define CHA_DEFAULT   0
#define CHA_HIGHLIGHT (1<<0)
#define CHA_BOLD      (1<<1)
#define CHA_DISABLED  (1<<2)

#define CON_DEFAULT      0
#define CON_AUTOREFRESH (1<<0)
#define CON_NOCURSOR    (1<<1)

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
	int flags;
	struct { int x, y; } cursor;
	attrchar_t * const data;
} console_t;

/**
 * The default console.
 */
extern console_t *stdcon;

/**
 * Initializes the console system.
 */
void console_init();

/**
 * Refreshs the console and rerenders the screen.
 */
void console_refresh();

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
 * Creates a new, sized console.
 * @param w The width of the console
 * @param h The height of the console
 * @remarks The size of the console is clamped to the screen size.
 */
console_t *console_create(int w, int h);

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
 * @remarks Overrides the character attributes.
 */
void console_setc(console_t *con, int x, int y, char c);

/**
 * Sets the character attributes on the console.
 */
void console_seta(console_t *con, int x, int y, int attribs);

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
int console_printf(console_t *con, char const * fmt, ...);

/**
 * Writes a given text with a given length to the console.
 */
void console_write(console_t *con, char const * text, int length);

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
int printf(char const * fmt, ...);

/**
 * Writes text to the console.
 */
void conwrite(char const * text, int length);