#pragma once

#include "string.h"
#include "pmm.h"

#include <stddef.h>

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

static inline void *malloc(size_t size) {
	(void)size;
	return pmm_getptr(pmm_alloc());
}

static inline void free(void *ptr) {
	pmm_free(pmm_getpage(ptr));
}