#include "interpreter.h"
#include "io.h"
#include "standard.h"
#include "shell.h"
#include "editor.h"
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

value_t getarg(int argc, value_t *argv, int i)
{
	if(argc <= i)
		basic_error(ERR_ARG_COUNT);
	return argv[i];
}

#define STR(n) basic_getstr(getarg(argc, argv, n))
#define NUM(n) basic_getnum(getarg(argc, argv, n))

#define IMPORT(x) x
#define FUNCTION(name, synpsis, desc, code) static value_t fun_##name(int argc, value_t *argv) { code return basic_mknull(); }
#define ORDER(mode, name, synpsis, desc, code) static value_t ord_##name(int argc, value_t *argv) { code return basic_mknull(); }
#include "stdlib.lst"
#undef ORDER
#undef FUNCTION
#undef IMPORT

static struct reg {
	char *name;
	int type;
	int mode;
	basfunc_f fn;
} functions[] = {

#define DEFAULT 0
#define NOPRG   (1<<0)

#define FUNCTION(name, synpsis, desc, code) { #name, BASIC_FUNCTION, DEFAULT, &fun_##name },
#define ORDER(mode, name, synpsis, desc, code) { #name, BASIC_ORDER, mode, &ord_##name },
#include "stdlib.lst"
#undef FUNCTION
	
	{ NULL, 0, DEFAULT, NULL },
};

void stdlib_init()
{
	struct reg * it = functions;
	
	while(it->name)
	{
		basic_register(it->name, it->fn, it->type);
		it++;
	}
}