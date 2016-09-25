#include "pmm.h"
#include "config.h"

#define BITMAP_LENGTH (PMM_MEMSIZE / PMM_PAGESIZE) / 32

/**
 * A bitmap where each set bit marks a free page.
 */
static uint32_t bitmap[BITMAP_LENGTH];

page_t pmm_getpage(void *ptr)
{
	return ((uint32_t)ptr) / PMM_PAGESIZE;
}

void *pmm_getptr(page_t page)
{
	return (void*)(page * PMM_PAGESIZE);
}

void pmm_mark(page_t page, int state)
{
	// This is important! We don't want the first page to be used ever
	if(page == 0) state = PMM_USED;

  uint32_t idx = page / 32;
  uint32_t bit = page % 32;
  
	if(state != PMM_FREE)
		bitmap[idx] &= ~(1<<bit);
	else 
		bitmap[idx] |= (1<<bit);
}

page_t pmm_alloc()
{
  for(uint32_t idx = 0; idx < BITMAP_LENGTH; idx++) {
    // fast skip when no bit is set
    if(bitmap[idx] == 0) {
      continue;
    }
    for(uint32_t bit = 0; bit < 32; bit++) {
      uint32_t mask = (1<<bit);
      if((bitmap[idx] & mask) == 0) {
        // If bit is not set, ignore the bit.
        continue;
      }
      
      // Mark the selected bit as used.
      bitmap[idx] &= ~mask;
      
      return 32 * idx + bit;
    }
  }
	// TODO: pmm_alloc failed, what do?!
  while(true);
}

/**
 * Frees an allocated page.
 */
void pmm_free(page_t page)
{
	pmm_mark(page, PMM_FREE);
}