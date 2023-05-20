#include "gc.h"
#include "debug.h"
#include "standard.h"

typedef struct gcrefref
{
	gcref_t * ptr;
	struct gcrefref * next;
} gcrefref_t;

static object_t * list = NULL;
static gcrefref_t * refs = NULL;

object_t * gc_alloc(int class, unsigned size, dtor_t dtor)
{
	static int objcnt = 0;
	object_t * obj = malloc(sizeof(object_t) + size);
	
	*obj = (object_t) {
		.data = (char*)obj + sizeof(object_t),
		.size = size,
		.class = class,
		.name = ++objcnt,
		.flags = OBJ_DEFAULT,
		.dtor = dtor,
		.next = list,
	};
	// Garbage-collected objects are zero-initialized.
	mem_set(obj->data, 0, size);
	list = obj;
	
	return obj;
}

void gc_addref(gcref_t * ptr)
{
	gcrefref_t * rr = malloc(sizeof(gcrefref_t));
	rr->ptr = ptr;
	 
	rr->next = refs;
	refs = rr;
}

void gc_rmref(gcref_t * ptr)
{
	gcrefref_t * ref = NULL;
	for(ref = refs; ref != NULL; ref = ref->next)
	{
		if(ref->ptr == ptr)
			break;
	}
	if(ref == NULL)
		return;

	if(ref == refs) {
		refs = ref->next;
	}
	else {
		for(gcrefref_t * r = refs; r != NULL; r = r->next) 
		{
			if(r->next != ref)
				continue;
			r->next = ref->next;
		}
		ref->next = NULL;
	}
}

void gc_collect()
{
	// Reset all object references
	for(object_t *o = list; o != NULL; o = o->next)
	{
		o->flags &= ~OBJ_HASREF;
	}
	
	// For each registered reference, mark the references
	// object used.
	for(gcrefref_t * r = refs; r != NULL; r = r->next)
	{
		if((*r->ptr) != NULL) {
			(*r->ptr)->flags |= OBJ_HASREF;
		}
	}
	
	// Delete all unreferenced objects
	int num = 0;
	{
		object_t * o = list;
		object_t * p = NULL;
		while(o != NULL)
		{
			if((o->flags & OBJ_HASREF) == 0)
			{
				num++;
				object_t * d = o->next;
				
				if(o->dtor != NULL) {
					(o->dtor)(o->data);
				}
				mem_set(o, 0, sizeof(object_t));
				free(o);
				
				if(list == o) {
					list = d;
				}
				
				o = d;
				if(p) { 
					p->next = d;
				}
			}
			else 
			{
				p = o;
				o = o->next;
			}
		}
	}
	debug("GC: Collected %d objects.\n", num);
}
