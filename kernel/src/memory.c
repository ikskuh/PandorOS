#include "memory.h"



void *memcpy(void *dst, void const *src, size_t len)
{
	mem_copy(dst, src, len);
	return dst;
}