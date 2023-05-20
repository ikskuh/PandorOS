#pragma once

#define OBJ_DEFAULT    0
#define OBJ_HASREF     (1<<0)

typedef void (*dtor_t)(void *obj);

typedef struct object
{
	void * data;
	int class;
	unsigned size;
	int flags;
	int name;
	dtor_t dtor;
	struct object * next;
} object_t;

/**
 * A reference to a garbage collector object.
 */
typedef object_t * gcref_t;

/**
 * Allocates a new garbage-collected object.
 * @param class A used-defined class value that will define the type of the object.
 * @param size  The size of the object in bytes.
 * @param dtor  An optional destructor for the object. Will be called when the object is collected.
 */
object_t * gc_alloc(int class, unsigned size, dtor_t dtor);

/**
 * Adds a static reference to the garbage collector.
 * This reference will be checked each gc_collect if it references a gcobject.
 * If an object is not referenced by any gcref, it will be collected.
 *
 * @param   ref The reference that should be added.
 * @remarks ref must not be invalid memory until it is removed by gc_rmref.
 */
void gc_addref(gcref_t * ref);

/**
 * Removes a previously added reference.
 */
void gc_rmref(gcref_t * ref);

/**
 * Collects all garbage.
 * This is done by checking all referenced object and deleting the rest.
 */
void gc_collect();
