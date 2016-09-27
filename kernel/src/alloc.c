#include "alloc.h"
#include "pmm.h"
#include "console.h"

struct allocator
{
	page_t page;
	size_t objectSize;
	uint8_t *bitmap;
	void *memstart;
};

allocator_t *allocator_new(size_t objectSize)
{
	page_t page = pmm_alloc();
	allocator_t *allocator = pmm_getptr(page);
	allocator->page       = page;
	allocator->objectSize = objectSize;
	allocator->bitmap     = (uint8_t*)pmm_getptr(page) + sizeof(allocator_t);
	allocator->memstart   = allocator->bitmap + (PMM_PAGESIZE / objectSize) / 8;
	
	for(int i = 0; i < objectSize; i += 8)
	{
		allocator->bitmap[i] = 0;
	}
	
	printf("new allocator (%d,%d,%d,%d)\n",
		page,
		objectSize,
		allocator->bitmap,
		allocator->memstart);
	
	return allocator;
}

void allocator_delete(allocator_t *alloc)
{
	pmm_free(alloc->page);
}

void *allocator_alloc(allocator_t *alloc)
{
	int objectCount = (PMM_PAGESIZE / alloc->objectSize);
	
	uint8_t *bitmap = alloc->bitmap;

	for(int i = 0; i < objectCount; i++)
	{
		int word = i / 8;
		int bit = i % 8;
		if(bitmap[word] & (1<<bit))
			continue;
		bitmap[word] |= (1<<bit);
		return (uint8_t*)alloc->memstart + alloc->objectSize * i;
	}
	
	return NULL;
}

void allocator_free(allocator_t *alloc, void *_ptr)
{
	int objectCount = (PMM_PAGESIZE / alloc->objectSize);
	
	uintptr_t ptr = (uintptr_t)_ptr - (uintptr_t)alloc->memstart;
	if(ptr >= (uintptr_t)alloc->memstart + alloc->objectSize * objectCount)
		return;
	
	uint8_t *bitmap = alloc->bitmap;
	
	int index = ptr / alloc->objectSize;
	int word = index / 8;
	int bit  = index % 8;
	bitmap[word] &= ~(1 << bit);
}