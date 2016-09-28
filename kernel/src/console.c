#include "console.h"
#include "printf.h"
#include "hal.h"
#include "pmm.h"
#include "menu.h"
#include <stdarg.h>
#include <stddef.h>

int screenwidth, screenheight;

console_t *stdcon = NULL;

#define CON_WANTS_REFRESH(con) ((con) == stdcon && (con)->flags && CON_AUTOREFRESH)

void console_init()
{
	hal_console_init(&screenwidth, &screenheight);
	
	// Render empty menu.
	menu_render(NULL);
}

void console_set(console_t *con)
{
	if(con != NULL)
		stdcon = con;
	console_refresh();
}

static void _setcursor(console_t *con)
{
	if(stdcon->flags & CON_NOCURSOR)
		hal_set_cursor(screenwidth, screenheight);
	else
		hal_set_cursor(con->cursor.x, con->cursor.y + 2);
}

void console_refresh()
{
	hal_render_console(stdcon, 0, 0, stdcon->width, stdcon->height);
	_setcursor(stdcon);
}

console_t *console_new()
{
	console_t *con = pmm_getptr(pmm_alloc());
	
	*con = (console_t) {
		// Consoles are 2 rows less tall than the screen.
		screenwidth, 
		screenheight - 2,
		CON_DEFAULT | CON_AUTOREFRESH,
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
	if(CON_WANTS_REFRESH(con))  {
		_setcursor(con);
		hal_render_console(con, 0, 0, con->width, con->height);
	}
}

void console_setcursor(console_t *con, int x, int y)
{
	con->cursor.x = x;
	con->cursor.y = y;
	if(CON_WANTS_REFRESH(con)) _setcursor(con);
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
	if(CON_WANTS_REFRESH(con))  hal_render_console(con, 0, 0, con->width, con->height);
}

void console_setc(console_t *con, int x, int y, char c)
{
	con->data[con->width * y + x] = (attrchar_t) { c, 0x00 };
	if(CON_WANTS_REFRESH(con))  hal_render_console(con, x, y, 1, 1);
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
		case '\b':
			if(con->cursor.x > 0) {
				con->cursor.x -= 1;
				console_setc(con, con->cursor.x, con->cursor.y, ' ');
			}
			break;
		default:
			console_setc(con, con->cursor.x, con->cursor.y, c);
			con->cursor.x += 1;
			if(con->cursor.x >= con->width) {
				console_newline(con);
			}
			break;
	}
	if(CON_WANTS_REFRESH(con))  _setcursor(con);
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