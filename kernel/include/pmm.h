#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PMM_FREE 0
#define PMM_USED 1

/**
 * A page index container.
 */
typedef uint32_t page_t;

/**
 * Gets the page the pointer is located in.
 */
page_t pmm_getpage(void *ptr);

/**
 * Gets the start pointer of the page.
 */
void *pmm_getptr(page_t page);

/**
 * Marks a given page as either used or free.
 */
void pmm_mark(page_t page, int status);

/**
 * Allocates a page.
 */
page_t pmm_alloc();

/**
 * Frees an allocated page.
 */
void pmm_free(page_t page);