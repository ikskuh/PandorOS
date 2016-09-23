#include "stdlib.h"

int int_to_string(char * str, int value, int base)
{
	if(base < 2) return 0;
	if(base > 16) return 0;
	int len = 0;
	if(value == 0)
	{
		str[0] = '0';
		str[1] = 0;
		return 1;
	}
	if(value < 0)
	{
		str[0] = '-';
		str++;
		value = -value;
		len += 1;
	}
	int i = 0;
	while(value > 0)
	{
		str[i++] = ("0123456789ABCDEF")[value % base];
		len++;
		value /= base;
	}
	str[i] = 0;
	for(int j = 0; j < i/2; j++)
	{
		char tmp = str[j];
		str[j] = str[i - j - 1];
		str[i - j - 1] = tmp;
	}
	return len;
}