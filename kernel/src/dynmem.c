#include "dynmem.h"
#include "standard.h"
#include <stddef.h>

dynmem_t dynmem_new()
{
	return (dynmem_t) {
		NULL, 0, 0
	};
}

void dynmem_write(dynmem_t * dm, void *ptr, int size)
{
	if(dm->cursor + size > dm->size)
	{
		dm->size += 0x100;
		void *nx = realloc(dm->ptr, dm->size);
		if(nx == NULL)
			; // TODO: Handle error
		dm->ptr = nx;
	}
	
	mem_copy(&dm->ptr[dm->cursor], ptr, size);
	
	dm->cursor += size;
}

void dynmem_free(dynmem_t * dm)
{
	free(dm->ptr);
}