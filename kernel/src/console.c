#include "console.h"
#include <stdarg.h>
#include <stddef.h>
#include "printf.h"
#include "hal.h"

static int width, height;

int cx, cy, yoffset;

static menu_entry_t const * menu;
static int menu_size;

static vchar mkvchar(char c, enum vchar_color color) {
	return (vchar) { c, color };
}

static enum vchar_color getMenuColor(menu_entry_t const * menu)
{
	if(menu->flags & MENU_RED) {
		if(menu->flags & MENU_SELECTED)
			return vcRedHighlight;
		else
			return vcRed;
	} else {
		if(menu->flags & MENU_SELECTED)
			return vcHighlight;
		else
			return vcDefault;
	}
}

void console_init()
{
	yoffset = 2;
	hal_console_init(&width, &height);
	height -= yoffset;
	cx = 0; cy = 0;
	menu_size = 0;
	menu = NULL;
	cls();
}

void console_move(int x, int y)
{
	cx = x;
	cy = y;
	hal_movecursor(x, y + 2);
}

static void render_menu()
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
	if(menu_size > 0)
	{
		int leftpad = 1;

		// MENU_DEFAULT, MENU_SELECTED ,MENU_RED, MENU_RIGHTALIGN
		for(int i = 0; i < menu_size; i++)
		{
			if((menu[i].flags & MENU_RIGHTALIGN) != 0) continue;
			
			enum vchar_color color = getMenuColor(&menu[i]);
			
			char const *str = menu[i].label;
			while(*str) {
				hal_setchar(leftpad++, 0, mkvchar(*str++, color));
			}
			hal_setchar(leftpad, 1, mkvchar(0xC1, vcDefault));
			hal_setchar(leftpad++, 0, mkvchar(0xB3, vcDefault));
		}
		
		int rightpad = width - 2;
		// MENU_DEFAULT, MENU_SELECTED ,MENU_RED, MENU_RIGHTALIGN
		for(int i = (menu_size - 1); i >= 0; i--)
		{
			if((menu[i].flags & MENU_RIGHTALIGN) == 0) continue;
			enum vchar_color color = getMenuColor(&menu[i]);

			char const *str = menu[i].label;
			int len = 0;
			while(*str++) len++;
			str = menu[i].label;
			for(int i = len - 1; i >= 0; i--) {
				hal_setchar(rightpad--, 0, mkvchar(str[i], color));
				len--;
			}
			hal_setchar(rightpad, 1, mkvchar(0xC1, vcDefault));
			hal_setchar(rightpad--, 0, mkvchar(0xB3, vcDefault));
		}
	}
}

void cls()
{
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			hal_setchar(x, y + yoffset, mkvchar(' ', vcDefault));
		}
	}
	render_menu();
}


void console_refresh()
{
	render_menu();
}

void console_setmenu(menu_entry_t const * entries, int count)
{
	menu = entries;
	if(menu != NULL)
		menu_size = count;
	else
		menu_size = 0;
	render_menu();
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
	switch(c)
	{
		case '\b':
			if(cx > 0)
				hal_setchar(--cx, cy + yoffset, mkvchar(' ', vcDefault));
			break;
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