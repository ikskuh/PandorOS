#pragma once

#include <stdbool.h>

int str_printf(char *str, char const *fmt, ...);

static inline bool str_eq(char const * lhs, char const * rhs)
{
	while(*lhs) {
		if(*rhs == 0) break;
		if((*lhs++) != (*rhs++))
			return false;
	}
	return (*lhs == 0) && (*rhs == 0);
}

static inline bool str_startswith(char const * str, char const * prefix)
{
	while(*prefix) {
		if((*prefix++) != (*str++))
			return false;
	}
	return true;
}

static inline void str_copy(char * dst, char const * src)
{
	while(((*dst++) = (*src++)));
}

static inline int str_len(char const *str)
{
	int len = 0;
	while(*str++) len++;
	return len;
}