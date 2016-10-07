#pragma once

#include <stdint.h>

typedef struct dynmem
{
	uint8_t *ptr;
	int cursor;
	int size;
} dynmem_t;

dynmem_t dynmem_new();

void dynmem_write(dynmem_t * dm, void *ptr, int size);

void dynmem_free(dynmem_t * dm);