#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OBJ_DEFAULT    0
#define OBJ_HASREF     (1<<0)

#define REF_DEFAULT    0
#define REF_REGISTERED (1<<0)

typedef void (*dtor_t)(void *obj);

typedef struct object
{
	void * data;
	int size;
	int flags;
	int name;
	dtor_t dtor;
	struct object * next;
} object_t;

typedef struct gcref
{
	char * name;
	object_t * obj;
	int flags;
	struct gcref * next;
} gcref_t;

object_t * list = NULL;
gcref_t * refs = NULL;

object_t * gc_alloc(int size, dtor_t dtor)
{
	static int objcnt = 0;
	object_t * obj = malloc(sizeof(object_t) + size);
	obj->data = (char*)obj + sizeof(object_t);
	obj->size = size;
	obj->name = ++objcnt;
	obj->flags = OBJ_DEFAULT;
	obj->dtor = dtor;
	obj->next = list;
	list = obj;
	
	return obj;
}

void gc_addref(gcref_t * ref)
{
	if(ref->flags & REF_REGISTERED)
		return;
	
	ref->next = refs;
	refs = ref;
	
	ref->flags |= REF_REGISTERED;
}

void gc_rmref(gcref_t * ref)
{
	if((ref->flags & REF_REGISTERED) == 0)
		return;
	
	if(ref == refs) {
		refs = ref->next;
	}
	else {
		for(gcref_t * r = refs; r != NULL; r = r->next) 
		{
			if(r->next != ref)
				continue;
			r->next = ref->next;
		}
		ref->next = NULL;
	}
	
	ref->flags &= ~REF_REGISTERED;
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
	for(gcref_t * r = refs; r != NULL; r = r->next)
	{
		if(r->obj != NULL) {
			r->obj->flags |= OBJ_HASREF;
		}
	}
	
	// Delete all unreferenced objects
	{
		object_t * o = list;
		object_t * p = NULL;
		while(o != NULL)
		{
			if((o->flags & OBJ_HASREF) == 0)
			{
				object_t * d = o->next;
				
				if(o->dtor != NULL) {
					(o->dtor)(o->data);
				}
				memset(o, 0, sizeof(object_t));
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
}

int main(int argc, char ** argv)
{
	gcref_t a = { "a" };
	gcref_t b = { "b" };
	gc_addref(&a);
	gc_addref(&b);
	
	gc_alloc(10, NULL);
	a.obj = gc_alloc(20, NULL);
	b.obj = gc_alloc(30, NULL);
	gc_alloc(40, NULL);
	gc_alloc(50, NULL);
	
	gc_collect();
	
	a.obj = b.obj;
	gc_collect();
	
	a.obj = gc_alloc(10, NULL);
	a.obj = gc_alloc(10, NULL);
	b.obj = gc_alloc(10, NULL);
	b.obj = gc_alloc(10, NULL);
	
	gc_collect();
	
	gc_rmref(&a);
	gc_rmref(&b);
	
	return 0;
}