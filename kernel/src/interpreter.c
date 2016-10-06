#include "interpreter.h"
#include "io.h"
#include "pmm.h"
#include "longjmp.h"
#include "standard.h"
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

void basic_compile(file_t * in, file_t * outfile)
{
	char * input = file_data(in);
	int size = file_size(in);

	for(int cursor = 0; cursor < size; )
	{
		// Skip all empty newlines
		while(input[cursor] == '\n')
		{
			cursor++;
			if(cursor >= size)
				break;
		}
		if(cursor >= size)
			break; // Double break!
		
		struct token token = lex(&input[cursor]);
			
		static char buffer[64];
		mem_set(buffer, 0, sizeof(buffer));
		mem_copy(buffer, input + cursor, token.length);
		
		printf("'%s'(%d)*%d = %d\n", buffer, input[cursor], token.length, token.type);
			
		if(token.type >= 0)
		{
			// token_t currtok;
			
			switch(token.type)
			{
				case TOK_INTEGER:
				{
					// currtok.val = basic_mknum(str_to_int(buffer, 10));
					break;
				}
				case TOK_BOOL:
				{
					// currtok.val = basic_mknum(str_eq(buffer, "True") || str_eq(buffer, "On"));
					break;
				}
				case TOK_STRING:
				{
					// Initialize with uninitialized pointer
					
					// char *str = basic_alloc(token.length - 1);
					// mem_copy(str, buffer + 1, token.length - 2);
					// str[token.length - 2] = 0;
					
					// currtok.val = basic_mkstr(str);
					
					break;
				}
				case TOK_VAR:
				{
					// currtok.var = var_byname(buffer);
					break;
				}
				case TOK_FUN:
				{
					// currtok.fun = basic_getfunc(BASIC_FUNCTION, buffer);
					// if(currtok.fun == NULL) {
					// 	basic_error(ERR_FUNC_NOT_FOUND);
					// }
					break;
				}
				case TOK_ORDER:
				{
					// for(int i = 0; buffer[i]; i++)
					// {
					// 	if(buffer[i] == ' ')
					// 		buffer[i] = 0;
					// }
					// currtok.fun = basic_getfunc(BASIC_ORDER, buffer);
					// if(currtok.fun == NULL) {
					// 	basic_error(ERR_FUNC_NOT_FOUND);
					// }
					break;
				}
			}
		
			if(token.type == 0)
				break;
		}
		else if(token.type == -2)
		{
			basic_error(ERR_INVALID_TOKEN);
		}
		cursor += token.length;
		
		if (input[cursor] == '\n' || cursor >= size)
		{
			// End of line/file:
		
			// Write End-Of-Line-Token:
			printf("END OF LINE\n");
			
			cursor++;
			
			if(cursor >= size)
				break;
		}
	}
	
}