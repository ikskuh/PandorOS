#pragma once

#include <stddef.h>

typedef struct allocator allocator_t;

/**
 * Creates a new allocator.
 * @param objectSize The size of the objects to be allocated.
 */
allocator_t *allocator_new(size_t objectSize);

/**
 * Destroys an allocator and invalidates all allocated objects.
 */
void allocator_delete(allocator_t *alloc);

/**
 * Allocates a new object.
 */
void *allocator_alloc(allocator_t *);

/**
 * Frees an object.
 */
void allocator_free(allocator_t *, void *);