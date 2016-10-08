#pragma once

#include <stdbool.h>
#include "ctype.h"

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

static inline bool str_eqi(char const * lhs, char const * rhs)
{
	while(*lhs) {
		if(*rhs == 0) break;
		if(tolower(*lhs++) != tolower(*rhs++))
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

static inline void str_cat(char * dst, char const * src)
{
	while(*dst++);
	dst--;
	while(((*dst++) = (*src++)));
}

static inline int str_len(char const *str)
{
	int len = 0;
	while(*str++) len++;
	return len;
}

static inline int str_comp(char const * lhs, char const * rhs)
{
	int s1;
	int s2;
	do {
		s1 = *lhs++;
		s2 = *rhs++;
		if (s1 == 0)
			break;
	} while (s1 == s2);
	return (s1 < s2) ? -1 : (s1 > s2);
}