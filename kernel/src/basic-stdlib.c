#include "interpreter.h"
#include "io.h"

#include <stddef.h>


void assert_argc(int argc, int i)
{
	if(argc != i)
		basic_error(ERR_ARG_COUNT);
}

void assert_type(value_t val, int type)
{
	if(val.type != type)
		basic_error(ERR_INVALID_ARG);
}

#define FUNCTION(name, synpsis, desc, code) static value_t name(int argc, value_t *argv) \
	code
#include "stdlib.lst"
#undef FUNCTION

static struct reg {
	char *name;
	basfunc_f fn;
} functions[] = {

#define FUNCTION(name, synpsis, desc, code) { #name, &name },
#include "stdlib.lst"
#undef FUNCTION
	
	{ NULL, NULL },
};

void stdlib_init()
{
	struct reg * it = functions;
	
	while(it->name)
	{
		basic_register(it->name, it->fn);
		it++;
	}
}