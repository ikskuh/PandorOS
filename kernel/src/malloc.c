#include "malloc.h"
#include "tlsf.h"
#include "pmm.h"
#include "io.h"

tlsf_t tlsf;


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
		printf("Out of memory!\n");
	}
	
	for(page_t p = lower; p <= upper; p++) {
		pmm_mark(p, PMM_USED);
	}
	
	size_t len = (size_t)(upper - lower + 1) * (size_t)PMM_PAGESIZE;
	
	printf("Heap is [%d, %d]\n", lower, upper);
	
	tlsf = tlsf_create_with_pool(pmm_getptr(lower), len);
}

void * malloc(size_t bytes)
{
	return tlsf_malloc(tlsf, bytes);
}


void free(void *ptr)
{
	tlsf_free(tlsf, ptr);
}


void * realloc(void* ptr, size_t size)
{
	return tlsf_realloc(tlsf, ptr, size);
}