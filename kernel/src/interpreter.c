#include "interpreter.h"
#include "io.h"
#include "pmm.h"
#include "longjmp.h"
#include "standard.h"
#include "debug.h"
#include "basic/lexer.h"

// declared in grammar.lg
extern bool errorhandler_valid;
extern jmp_buf errorhandler;
	
/**
 * Throws an error.
 */
void basic_error(error_t reason)
{
	if(reason == ERR_SUCCESS) return;
	if(errorhandler_valid == false) {
		printf("Critical fault: ");
		switch(reason)
		{
	#define ERR(i,n) case ERR_##n: \
		printf("%s\n", #n); \
		break; 
	#include "basic/errors.lst"
	#undef ERR
			default:
				printf("ERR_UNKNOWN\n");
				break;
		}
		while(1);
	} else {
		longjmp(errorhandler, reason);
		printf("?longjmp?");
	}
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
	mem_set(basicmemory, '?', PMM_PAGESIZE);
}

void *basic_alloc(size_t size)
{
	void *ptr = basicmemory;
	basicmemory += size;
	return ptr;
}

char const * basic_err_to_string(error_t err)
{
	switch(err)
	{
#define ERR(i,n) case ERR_##n: return #n;
#include "basic/errors.lst"
#undef ERR
	default: return "UNKNWON";
	}
}

basfunc_f basic_getfunc(int type, char const *name);

dynmem_t basic_compile(char const * input, int insize)
{
	char zero = 0;

	dynmem_t bytecode = dynmem_new();
	
	for(int rcursor = 0; rcursor < insize; )
	{
		// Skip all empty newlines
		while(input[rcursor] == '\n')
		{
			rcursor++;
			if(rcursor >= insize)
				break;
		}
		if(rcursor >= insize)
			break; // Double break!
		
		struct token token = lex(&input[rcursor]);
		
		if(token.type >= 0 || token.type == TOKEN_EOL)
		{
			char buffer[512];
			mem_set(buffer, 0, sizeof(buffer));
			mem_copy(buffer, input + rcursor, token.length);
			
			{
				char tok = token.type;
				dynmem_write(&bytecode, &tok, 1); 
			}
			
			switch(token.type)
			{
				case TOK_INTEGER:
				{
					int ival = str_to_int(buffer, 10);
					dynmem_write(&bytecode, &ival, sizeof(ival));
					break;
				}
				case TOK_BOOL:
				{
					bool bval = str_eq(buffer, "True") || str_eq(buffer, "On");
					dynmem_write(&bytecode, &bval, sizeof(bval));
					break;
				}
				// Both strings are stored with each length+ptr and zero termination.
				case TOK_STRING:
				{
					int len = token.length - 2;
					dynmem_write(&bytecode, &len, sizeof(len));
					dynmem_write(&bytecode, buffer + 1, len);
					dynmem_write(&bytecode, &zero, sizeof(zero));
					break;
				}
				// Both strings are stored with each length+ptr and zero termination.
				case TOK_FUN:
				case TOK_VAR:
				case TOK_ORDER:
				{
					int len = token.length;
					dynmem_write(&bytecode, &len, sizeof(len));
					dynmem_write(&bytecode, buffer, len);
					dynmem_write(&bytecode, &zero, sizeof(zero));
					break;
				}
			}
			if(token.type == TOKEN_EMPTY)
				break;
		}
		else if(token.type == TOKEN_INVALID)
		{
			dynmem_free(&bytecode);
			basic_error(ERR_INVALID_TOKEN);
		}
		rcursor += token.length;
		
		if (input[rcursor] == '\n' || rcursor >= insize)
		{
			// End of line/file:
		
			char end;
			// Write End-Of-Line-Token:
			if(rcursor >= insize) {
				end = TOKEN_EOF;
			} else {
				end = TOKEN_EOL;
			}
			dynmem_write(&bytecode, &end, 1);
			
			rcursor++;
			
			if(rcursor >= insize)
				break;
		}
	}
	
	// hexdump("Tokenized Code", bytecode.ptr, bytecode.cursor);
	
	return bytecode;
}


value_t basic_execute(char const *input)
{
	int len = str_len(input);
	
	dynmem_t bytecode = basic_compile(input, len);
	
	value_t result = basic_execute2(bytecode.ptr, bytecode.cursor);
	
	dynmem_free(&bytecode);
	
	return result;
}


	
struct basreg {
	char name[64];
	int type;
	basfunc_f func;
};

static struct basreg functions[1024];
static int basfunc_cnt = 0;

void basic_register(char const *name, basfunc_f function, int type)
{
	functions[basfunc_cnt] = (struct basreg){ "", type, function };
	
	str_copy(functions[basfunc_cnt].name, name);
	if(type == BASIC_FUNCTION)
		str_cat(functions[basfunc_cnt].name, "("); // Fancy hack to find the tokens instead of the names.
	
	++basfunc_cnt;
}

basfunc_f basic_getfunc(int type, char const *name)
{
	for(int i = 0; i < basfunc_cnt; i++)
	{
		if(functions[i].type != type)
			continue;
		if(str_eq(name, functions[i].name))
			return functions[i].func;
	}
	return NULL;
}