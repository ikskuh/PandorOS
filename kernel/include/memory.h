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

static inline int mem_comp(const void *lhs, const void *rhs, size_t len)
{
	if(len == 0) return 0;
	char const * l = lhs;
	char const * r = rhs;
	while(len-- > 0)
	{
		if (*l != *r) {
			return (*l - *r);
		}
		l++, r++;
	}
	return 0;
}
