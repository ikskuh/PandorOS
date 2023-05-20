#include "interpreter.h"
#include "io.h"
#include "standard.h"
#include "shell.h"
#include "editor.h"
#include "hexedit.h"
#include "file.h"
#include "options.h"
#include "debug.h"
#include "gc.h"
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
#define PTR(n,t) basic_getptr(getarg(argc, argv, n), t)

#define IMPORT(x) x
#define FUNCTION(name, synpsis, desc, code) static value_t fun_##name(int argc, value_t *argv) { code return basic_mknull(); }
#define ORDER(mode, name, synpsis, desc, code) static value_t ord_##name(int argc, value_t *argv) { code return basic_mknull(); }
#include "stdlib.lst"
#undef ORDER
#undef FUNCTION
#undef IMPORT


void stdlib_init()
{
#define FUNCTION(name, synpsis, desc, code) basic_register(#name, &fun_##name);
#define ORDER(mode, name, synpsis, desc, code) basic_register(#name, &ord_##name);
#include "stdlib.lst"
#undef FUNCTION
#undef ORDER
}