#include "string.h"
#include "printf.h"
#include <stdarg.h>
#include <stddef.h>


static char *str_printf_str;
static int str_printf_cur;

void str_printf_putc(char c)
{
	str_printf_str[str_printf_cur] = c;
	str_printf_cur += 1;
	str_printf_str[str_printf_cur] = 0;
}

int str_printf(char *str, char const *fmt, ...)
{
	int result;
	str_printf_str = str;
	str_printf_cur = 0;
	
	va_list list;
	va_start(list, fmt);
	if(str != NULL)
		result = gprintf(str_printf_putc, fmt, list);
	else
		result = gprintf(NULL, fmt, list);
	va_end(list);
	
	str_printf_str = NULL;
	str_printf_cur = 0;
	
	return result;
}