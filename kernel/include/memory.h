#pragma once

#include <stddef.h>

static inline void mem_copy(void *dst, void const * src, size_t len)
{
	char *d = dst;
	char const *s = src;
	for(size_t i = 0; i < len; i++) {
		d[i] = s[i];
	}
}

static inline void mem_set(void *dst, int value, size_t len)
{
	char *d = dst;
	for(size_t i = 0; i < len; i++) {
		d[i] = value;
	}
}