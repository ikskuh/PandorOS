#include "interpreter.h"
#include "io.h"

#include <stddef.h>

#define FUNCTION(name) static value_t name(int argc, value_t *argv)

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

FUNCTION(Abs)
{
	assert_argc(argc, 1);
	number_t num = basic_getnum(argv[0]);
	if(num)
		return basic_mknum(-num);
	else
		return basic_mknum(num);
}

FUNCTION(Print)
{
	for(int i = 0; i < argc; i++)
	{
		if(i > 0) printf(" ");
		printf("%v", argv[i]);
	}
	printf("\n");
	return basic_mknull();
}

FUNCTION(Sum)
{
	number_t sum = 0;
	for(int i = 0; i < argc; i++)
	{
		sum += basic_getnum(argv[i]);
	}
	return basic_mknum(sum);
}

FUNCTION(Product)
{
	number_t sum = 1;
	for(int i = 0; i < argc; i++)
	{
		sum *= basic_getnum(argv[i]);
	}
	return basic_mknum(sum);
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