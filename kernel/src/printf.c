#include <stdarg.h>
#include "standard.h"
#include "debug.h"
#include "interpreter.h"

static char buffer[128];

int gprintf(void (*_putc)(char c), char const *fmt, va_list list)
{
	int chars = 0;
#define PUTC(C) do { \
		char __c = (C); \
		if(_putc != NULL) { \
			_putc(__c); \
		} \
		chars++; \
		} while(false)
	
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
					PUTC(c);
					break;
				}
				case 'd':
				{
					int val = va_arg(list, int);
					int len = int_to_string(buffer, val, 10);
					for(int i = 0; i < len; i++) {
						PUTC(buffer[i]);
					}
					break;
				}
				case 'b':
				{
					int val = va_arg(list, int);
					int len = int_to_string(buffer, val, 2);
					for(int i = 0; i < len; i++) {
						PUTC(buffer[i]);
					}
					break;
				}
				case 'x':
				case 'X':
				{
					int val = va_arg(list, int);
					int len = int_to_string(buffer, val, 16);
					for(int i = 0; i < len; i++) {
						PUTC(buffer[i]);
					}
					break;
				}
				case 's':
				{
					char const *str = va_arg(list, char const *);
					while(*str) {
						PUTC(*str++);
					}
					break;
				}
				case 'v':
				{
					value_t val = va_arg(list, value_t);
					switch(val.type)
					{
						case TYPE_NULL:
							PUTC('N');
							PUTC('U');
							PUTC('L');
							PUTC('L');
							break;
						case TYPE_NUM:
						{
							int len = int_to_string(buffer, val.number, 10);
							for(int i = 0; i < len; i++) {
								PUTC(buffer[i]);
							}
							break;
						}
						case TYPE_PTR:
						{
							PUTC('0');
							PUTC('x');
							int len = int_to_string(buffer, (int)val.pointer.ref, 16);
							for(int i = 0; i < len; i++) {
								PUTC(buffer[i]);
							}
							PUTC('\'');
							len = int_to_string(buffer, val.pointer.type, 10);
							for(int i = 0; i < len; i++) {
								PUTC(buffer[i]);
							}
							break;
						}
						case TYPE_TEXT:
						{
							char const *str = val.string;
							while(*str) {
								PUTC(*str++);
							}
							break;
						}
					}
					break;
				}
				default:
					PUTC(c);
					break;
			}
		}
		else
		{
			PUTC(c);
		}
	}
#undef PUTC
	return chars;
}