#pragma once

#include <stdint.h>

#define CHAR_DEFAULT 0

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

extern console_t *stdcon;

void console_init();

void console_set(console_t *con);

/**
 * Creates a new console.
 */
console_t *console_new();

/**
 * Destroys a console.
 */
void console_delete(console_t *con);

void console_clear(console_t *con);

void console_scroll(console_t *con, int lines);

void console_setc(console_t *con, int x, int y, char c);
void console_putc(console_t *con, char c);
void console_puts(console_t *con, char const * str);
void console_printf(console_t *con, char const * fmt, ...);

void cls();
void putc(char c);
void puts(char const * str);
void setc(int x, int y, char c);
void printf(char const * fmt, ...);
