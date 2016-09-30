#include "interpreter.h"
#include "io.h"
#include "pmm.h"

/**
 * Throws an error.
 */
void basic_error(error_t reason)
{
	switch(reason)
	{
#define ERR(i,n) case ERR_##n: \
	printf("Fault: %s\n", #n); \
	break; 
#include "basic/errors.lst"
#undef ERR
		default:
			printf("Failt: ERR_UNKNOWN\n");
			break;
	}
	while(1);
}

static page_t stringpage;

void basic_init()
{
	stringpage = pmm_alloc();
}

bool basic_isnull(value_t value)
{
	return (value.type == TYPE_NULL);
}

number_t basic_getnum(value_t value)
{
	if(value.type != TYPE_NUM) basic_error(ERR_INVALID_ARG);
	return value.number;
}

string_t basic_getstr(value_t value)
{
	if(value.type != TYPE_TEXT) basic_error(ERR_INVALID_ARG);
	return value.string;
}

value_t basic_mknum(number_t num)
{
	value_t val;
	val.type = TYPE_NUM;
	val.number = num;
	return val;
}

value_t basic_mkstr(string_t str)
{
	value_t val;
	val.type = TYPE_TEXT;
	val.string = str;
	return val;
}

value_t basic_mknull()
{
	value_t val;
	val.type = TYPE_NULL;
	return val;
}

static char *basicmemory;

void basic_memreset()
{
	basicmemory = pmm_getptr(stringpage);
}

void *basic_alloc(size_t size)
{
	void *ptr = basicmemory;
	basicmemory += size;
	return ptr;
}
