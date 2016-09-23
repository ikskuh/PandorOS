#include "console.h"
#include <stdarg.h>
#include "printf.h"
#include "hal.h"

void console_init()
{
	hal_console_init();
	cls();
}

void console_move(int x, int y)
{
	hal_movecursor(x, y);
}

void cls()
{
	hal_cls();
}

void puts(char const *string)
{
	while(*string)
	{
		putc(*string++);
	}
}

void putc(char c)
{
	// TODO: Call HAL function
	hal_putc(c);
}

int printf(char const *fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	int val = gprintf(putc, fmt, list);
	va_end(list);
	return val;
}