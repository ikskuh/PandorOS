#pragma once

#include <stddef.h>



/**
 * Initializes the malloc allocator.
 */
void malloc_init();


/**
 * Allocates bytes of memory.
 * @param len The number of bytes to allocate.
 */
void * malloc(size_t len);

/**
 * Allocates zero filled bytes of memory.
 * @param len The number of bytes to allocate.
 */
void * zalloc(size_t len);

/**
 * Frees previously allocated memory.
 */
void free(void *ptr);


/**
 * Reallocates a given pointer with a new size.
 */
void * realloc(void* ptr, size_t size);