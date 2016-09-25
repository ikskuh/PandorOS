#include "console.h"
#include "printf.h"
#include "hal.h"
#include "pmm.h"
#include <stdarg.h>
#include <stddef.h>

static int screenwidth, screenheight;

console_t *stdcon = NULL;

void console_init()
{
	hal_console_init(&screenwidth, &screenheight);
	
	// Render empty menu.
	console_menu(NULL);
}

void console_set(console_t *con)
{
	if(con != NULL)
		stdcon = con;
	hal_render_console(stdcon, 0, 0, con->width, con->height);
	hal_set_cursor(con->cursor.x, con->cursor.y);
}

console_t *console_new()
{
	console_t *con = pmm_getptr(pmm_alloc());
	
	*con = (console_t) {
		// Consoles are 2 rows less tall than the screen.
		screenwidth, 
		screenheight - 2,
		{ 0, 0 },
		(attrchar_t*)((uint8_t*)con + sizeof(*con)),
	};
	
	console_clear(con);
	
	return con;
}

void console_delete(console_t *con)
{
	pmm_free(pmm_getpage(con));
}

void console_clear(console_t *con)
{
	for(int y = 0; y < con->height; y++) {
		for(int x = 0; x < con->width; x++) {
			con->data[con->width * y + x] = (attrchar_t) { ' ', 0x00 };
		}
	}
	con->cursor.x = 0;
	con->cursor.y = 0;
	if(con == stdcon) {
		hal_set_cursor(con->cursor.x, con->cursor.y);
		hal_render_console(con, 0, 0, con->width, con->height);
	}
}

void console_setcursor(console_t *con, int x, int y)
{
	con->cursor.x = x;
	con->cursor.y = y;
	if(con == stdcon) hal_set_cursor(con->cursor.x, con->cursor.y);
}

void console_scroll(console_t *con, int lines)
{
	for(int i = 0; i < lines; i++)
	{
		for(int y = 1; y < con->height; y++)
		{
			for(int x = 0; x < con->width; x++)
			{
				con->data[con->width * (y-1) + x] = con->data[con->width * y + x];
			}
		}
		for(int x = 0; x < con->width; x++)
		{
			con->data[con->width * (con->height - 1) + x] = (attrchar_t) { ' ', 0x00 };
		}
		con->cursor.y -= 1;
	}
	if(con == stdcon) hal_render_console(con, 0, 0, con->width, con->height);
}

void console_setc(console_t *con, int x, int y, char c)
{
	con->data[con->width * y + x] = (attrchar_t) { c, 0x00 };
	if(con == stdcon) hal_render_console(con, x, y, 1, 1);
}

void console_newline(console_t *con)
{
	con->cursor.x = 0;
	con->cursor.y += 1;
	while(con->cursor.y >= con->height)
	{
		console_scroll(con, 1);
	}
}

void console_putc(console_t *con, char c)
{
	switch(c)
	{
		case '\r':
			break;
		case '\n':
			console_newline(con);
			break;
		default:
			console_setc(con, con->cursor.x, con->cursor.y, c);
			con->cursor.x += 1;
			if(con->cursor.x >= con->width) {
				console_newline(con);
			}
			break;
	}
	if(con == stdcon) hal_set_cursor(con->cursor.x, con->cursor.y);
}

void console_puts(console_t *con, char const * str)
{
	while(*str) {
		console_putc(con, *str++);
	}
}

static console_t *printf_target;

static void printf_putc(char c)
{
	console_putc(printf_target, c);
}

void console_printf(console_t *con, char const * fmt, ...)
{
	printf_target = con;
	
	va_list list;
	va_start(list, fmt);
	gprintf(printf_putc, fmt, list);
	va_end(list);
	
	printf_target = NULL;
}



void cls()
{
	console_clear(stdcon);
}

void putc(char c)
{
	console_putc(stdcon, c);
}

void puts(char const * str)
{
	console_puts(stdcon, str);
}

void setc(int x, int y, char c)
{
	console_setc(stdcon, x, y, c);
}

void setcursor(int x, int y)
{
	console_setcursor(stdcon, x, y);
}

void printf(char const * fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	gprintf(putc, fmt, list);
	va_end(list);
}

static int menucolor(menuitem_t * const item)
{
	if(item->flags & MENU_RED)
	{
		if(item->flags & MENU_SELECTED)
			return CHA_RED | CHA_HIGHLIGHT;
		else
			return CHA_RED;
	}
	else
	{
		if(item->flags & MENU_SELECTED)
			return CHA_HIGHLIGHT;
		else
			return CHA_DEFAULT;
	}
}

void console_menu(menu_t const * menu)
{
	for(int x = 0; x < screenwidth; x++) {
		char c = ' ';
		if(x == 0)
			c = 0xB3;
		if(x == (screenwidth - 1))
			c = 0xB3;
		hal_render_raw(x, 0, c, CHA_DEFAULT);
		
		c = 0xC4;
		if(x == 0)
			c = 0xC0;
		if(x == (screenwidth - 1))
			c = 0xD9;
		hal_render_raw(x, 1, c, CHA_DEFAULT);
	}
	if(menu == NULL || menu->length <= 0)
		return;
		
	int leftpad = 1;

	// MENU_DEFAULT, MENU_SELECTED ,MENU_RED, MENU_RIGHTALIGN
	for(int i = 0; i < menu->length; i++)
	{
		menuitem_t *item = &menu->items[i];
		if((item->flags & MENU_RIGHTALIGN) != 0) continue;
		
		int col = menucolor(item);
		char const *str = item->label;
		while(*str) {
			hal_render_raw(leftpad++, 0, *str++, col);
		}
		hal_render_raw(leftpad, 1, 0xC1, CHA_DEFAULT);
		hal_render_raw(leftpad++, 0, 0xB3, CHA_DEFAULT);
	}
	
	int rightpad = screenwidth - 2;
	// MENU_DEFAULT, MENU_SELECTED ,MENU_RED, MENU_RIGHTALIGN
	for(int i = (menu->length - 1); i >= 0; i--)
	{
		menuitem_t *item = &menu->items[i];
		if((item->flags & MENU_RIGHTALIGN) == 0) continue;

		int col = menucolor(item);
		char const *str = item->label;
		int len = 0;
		while(*str++) len++;
		str = item->label;
		for(int i = len - 1; i >= 0; i--) {
			hal_render_raw(rightpad--, 0, str[i], col);
			len--;
		}
		hal_render_raw(rightpad, 1, 0xC1, CHA_DEFAULT);
		hal_render_raw(rightpad--, 0, 0xB3, CHA_DEFAULT);
	}
}