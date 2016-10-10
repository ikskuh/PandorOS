#pragma once

#include <stdbool.h>
#include "ctype.h"

/**
 * Copies a formatted string into a target
 * buffer.
 * @remarks Uses printf formatting.
 */
int str_printf(char *str, char const *fmt, ...);

/**
 * Compares two strings for equality.
 */
static inline bool str_eq(char const * lhs, char const * rhs)
{
	while(*lhs) {
		if(*rhs == 0) break;
		if((*lhs++) != (*rhs++))
			return false;
	}
	return (*lhs == 0) && (*rhs == 0);
}

/**
 * Compares two strings for equality
 * with case insensitivity.
 */
static inline bool str_eqi(char const * lhs, char const * rhs)
{
	while(*lhs) {
		if(*rhs == 0) break;
		if(tolower(*lhs++) != tolower(*rhs++))
			return false;
	}
	return (*lhs == 0) && (*rhs == 0);
}

/** 
 * Checks if a string starts with another string.
 */
static inline bool str_startswith(char const * str, char const * prefix)
{
	while(*prefix) {
		if((*prefix++) != (*str++))
			return false;
	}
	return true;
}

/**
 * Copies one string into another buffer.
 */
static inline void str_copy(char * dst, char const * src)
{
	while(((*dst++) = (*src++)));
}

/** 
 * Appends a string to another string.
 */
static inline void str_cat(char * dst, char const * src)
{
	while(*dst++);
	dst--;
	while(((*dst++) = (*src++)));
}

/**
 * Returns the length of a given string.
 */
static inline int str_len(char const *str)
{
	int len = 0;
	while(*str++) len++;
	return len;
}

/**
 * Compares two strings for equality and returns if 
 * one string is "larger" than another.
 */
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

/**
 * Hashes the given string.
 * @remarks Taken from http://www.cse.yorku.ca/~oz/hash.html
 */
static inline int str_hash(char const * str)
{
	int hash = 5381;
	int c;
	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;
	return hash;
}