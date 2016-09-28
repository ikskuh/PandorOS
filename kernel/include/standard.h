#pragma once

#include "string.h"

/**
 * Converts an integer to a string value.
 */
int int_to_string(char * str, int value, int base);

static inline int max(int a, int b)
{
	return (a > b) ? a : b;
}

static inline int min(int a, int b)
{
	return (a < b) ? a : b;
}