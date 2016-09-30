#include "interpreter.h"
#include "io.h"

#include <stddef.h>

#define FUNCTION(name) static value_t name(int argc, value_t *argv)

FUNCTION(Abs)
{
	if(argv[0] < 0)
		return -argv[0];
	else
		return argv[0];
}

FUNCTION(Print)
{
	for(int i = 0; i < argc; i++)
	{
		if(i > 0) printf(" ");
		printf("%d", argv[i]);
	}
	printf("\n");
	return 0;
}

FUNCTION(Sum)
{
	value_t sum = 0;
	for(int i = 0; i < argc; i++)
	{
		sum += argv[i];
		// printf("[%d] = %d\n", i, argv[i]);
	}
	return sum;
}

FUNCTION(Product)
{
	value_t sum = 1;
	for(int i = 0; i < argc; i++)
	{
		sum *= argv[i];
		// printf("[%d] = %d\n", i, argv[i]);
	}
	return sum;
}

#define REG(fn) { #fn, fn }

static struct reg {
	char *name;
	basfunc_f fn;
} functions[] = {

	REG(Abs),
	REG(Print),
	REG(Sum),
	REG(Product),
	
	
	{ NULL, NULL },
};

void basic_init()
{
	struct reg * it = functions;
	
	while(it->name)
	{
		basic_register(it->name, it->fn);
		it++;
	}
}