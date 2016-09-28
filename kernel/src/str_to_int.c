#include "standard.h"

int str_to_int(char *str, int base)
{
	int value = 0;
	int negative = 0;
	if(*str == '-')
	{
		negative = 1;
		str++;
	}
	while(*str)
	{
		value *= base;
		char c = *str++;
		int digit = -1;
		if(c >= '0' && c <= '9')
			digit = c - '0';
		else if(c >= 'a' && c <= 'f')
			digit = c - 'a' + 10;
		else if(c >= 'A' && c <= 'F')
			digit = c - 'A' + 10;
		else
			break;
		value += digit;
	}
	if(negative)
		return -value;
	else
		return value;
}