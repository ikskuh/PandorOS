#include "console.h"
#include <stdarg.h>
#include "printf.h"
#include "hal.h"

static int width, height;

int cx, cy, yoffset;

static vchar mkvchar(char c, enum vchar_color color) {
	return (vchar) { c, color };
}

void console_init()
{
	yoffset = 2;
	hal_console_init(&width, &height);
	height -= yoffset;
	cx = 0; cy = 0;
	cls();
}

void console_move(int x, int y)
{
	cx = x;
	cy = y;
	hal_movecursor(x, y + 2);
}

void cls()
{
	for(int x = 0; x < width; x++) {
		for(int y = 0; y < (yoffset - 1); y++) {
			char c = ' ';
			if(x == 0)
				c = 0xB3;
			if(x == (width - 1))
				c = 0xB3;
			hal_setchar(x, y, mkvchar(c, vcDefault));
		}
		char c = 0xC4;
		if(x == 0)
			c = 0xC0;
		if(x == (width - 1))
			c = 0xD9;
		hal_setchar(x, yoffset - 1, mkvchar(c, vcDefault));
	}
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			hal_setchar(x, y + yoffset, mkvchar(' ', vcDefault));
		}
	}
}

void puts(char const *string)
{
	while(*string)
	{
		putc(*string++);
	}
}

static void newline()
{
	cx = 0; cy += 1;
	if(cy >= height)
	{
		// scroll
		for(int y = 1; y < height; y++)
		{
			for(int x = 0; x < width; x++)
			{
				hal_setchar(x, y - 1 + yoffset, hal_getchar(x, y + yoffset));
			}
		}
		for(int x = 0; x < width; x++)
		{
			hal_setchar(x, height - 1 + yoffset, mkvchar(' ', vcDefault));
		}
		cy -= 1;
	}
}

void putc(char c)
{
	// TODO: Call HAL function
	switch(c)
	{
		case '\r': break;
		case '\n':
			newline();
			break;
		default:
			hal_setchar(cx++, cy + yoffset, mkvchar(c, vcDefault));
			if(cx >= width)
				newline();
			break;
	}
	hal_movecursor(cx, cy + yoffset);
}

int printf(char const *fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	int val = gprintf(putc, fmt, list);
	va_end(list);
	return val;
}