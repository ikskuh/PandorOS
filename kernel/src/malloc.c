#include "malloc.h"
#include "tlsf.h"
#include "pmm.h"
#include "debug.h"
#include "memory.h"
#include "interpreter.h"

tlsf_t tlsf = NULL;

void malloc_init()
{
	page_t lower = 0;
	page_t upper = 0;
	
	for(page_t p = PMM_PAGECOUNT - 1; p > 0; p--)
	{
		if(upper == 0) {
			if(pmm_isfree(p)) {
				upper = p;
			}
		} else {
			if(pmm_isfree(p) == false) {
				lower = p + 1;
				break;
			}
		}
	}
	if(upper <= lower) {
		debug("Out of memory!\n");
		while(1);
	}
	
	for(page_t p = lower; p <= upper; p++) {
		pmm_mark(p, PMM_USED);
	}
	
	size_t len = (size_t)(upper - lower + 1) * (size_t)PMM_PAGESIZE;
	
	debug("Heap is [%d, %d]\n", lower, upper);
	
	tlsf = tlsf_create_with_pool(pmm_getptr(lower), len);
}

void * malloc(size_t bytes)
{
	if(tlsf == NULL)
		basic_error(ERR_UNINITIALIZED);
	return tlsf_malloc(tlsf, bytes);
}

void * zalloc(size_t bytes)
{
	char * ptr = malloc(bytes);
	if(ptr != NULL)
		mem_set(ptr, 0, bytes);
	return ptr;
}

void free(void *ptr)
{
	if(tlsf == NULL)
		basic_error(ERR_UNINITIALIZED);
	tlsf_free(tlsf, ptr);
}


void * realloc(void* ptr, size_t size)
{
	if(tlsf == NULL)
		basic_error(ERR_UNINITIALIZED);
	return tlsf_realloc(tlsf, ptr, size);
}