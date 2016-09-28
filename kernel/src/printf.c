#include <stdarg.h>
#include "standard.h"

static char buffer[128];

int gprintf(void (*_putc)(char c), char const *fmt, va_list list)
{
	int chars = 0;
	while(*fmt)
	{
		char c = *fmt++;
		if(c == '%')
		{
			c = *fmt++;
			switch(c)
			{
				case 'c':
				{
					char c = va_arg(list, int);
					_putc(c);
					chars++;
					break;
				}
				case 'd':
				{
					int val = va_arg(list, int);
					int len = int_to_string(buffer, val, 10);
					for(int i = 0; i < len; i++) {
						_putc(buffer[i]);
					}
					chars += len;
					break;
				}
				case 'b':
				{
					int val = va_arg(list, int);
					int len = int_to_string(buffer, val, 2);
					for(int i = 0; i < len; i++) {
						_putc(buffer[i]);
					}
					chars += len;
					break;
				}
				case 'x':
				case 'X':
				{
					int val = va_arg(list, int);
					int len = int_to_string(buffer, val, 16);
					for(int i = 0; i < len; i++) {
						_putc(buffer[i]);
					}
					chars += len;
					break;
				}
				case 's':
				{
					char const *str = va_arg(list, char const *);
					while(*str) {
						_putc(*str++);
						chars++;
					}
					break;
				}
				default:
					_putc(c);
					chars++;
					break;
			}
		}
		else
		{
			_putc(c);
			chars++;
		}
	}
	return chars;
}