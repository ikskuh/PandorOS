#include <stdarg.h>
#include "standard.h"
#include "interpreter.h"

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
				case 'v':
				{
					value_t val = va_arg(list, value_t);
					switch(val.type)
					{
						case TYPE_NULL:
							_putc('N');
							_putc('U');
							_putc('L');
							_putc('L');
							break;
						case TYPE_NUM:
						{
							int len = int_to_string(buffer, val.number, 10);
							for(int i = 0; i < len; i++) {
								_putc(buffer[i]);
							}
							chars += len;
							break;
						}
						case TYPE_TEXT:
						{
							char const *str = val.string;
							while(*str) {
								_putc(*str++);
								chars++;
							}
							break;
						}
						case TYPE_CFLOW:
						{
							char const * msg;
							switch(val.cflow)
							{
								case CFLOW_WHILE: msg = "CFLOW(While,"; break;
								case CFLOW_IF: msg = "CFLOW(If,"; break;
								case CFLOW_THEN: msg = "CFLOW(Then,"; break;
								case CFLOW_END: msg = "CFLOW(End,"; break;
								case CFLOW_ELSE: msg = "CFLOW(Else,"; break;
								default: msg = "CFLOW(???,"; break;
							}
							while(*msg) _putc(*msg++);
							
							if(val.number != 0)
								_putc('1');
							else
								_putc('0');
							_putc(')');
							break;
						}
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