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

typedef struct dynmem
{
	char *ptr;
	int cursor;
	int size;
} dynmem_t;

static dynmem_t dm_new()
{
	return (dynmem_t) {
		NULL, 0, 0
	};
}

static void dm_write(dynmem_t * dm, void *ptr, int size)
{
	if(dm->cursor + size > dm->size)
	{
		debug("Resize %d to %d\n", dm->size, dm->size + 0x100);
		dm->size += 0x100;
		void *nx = realloc(dm->ptr, dm->size);
		if(nx == NULL)
			; // TODO: Handle error
		dm->ptr = nx;
		
	}
	
	mem_copy(&dm->ptr[dm->cursor], ptr, size);
	
	dm->cursor += size;
}

static void dm_kill(dynmem_t * dm)
{
	free(dm->ptr);
}

void basic_compile(file_t * infile, file_t * outfile)
{
	char zero = 0;
	char * input = file_data(infile);
	int insize = file_size(infile);

	dynmem_t bytecode = dm_new();
	
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
		if(token.type >= 0)
		{
			char buffer[512];
			mem_set(buffer, 0, sizeof(buffer));
			mem_copy(buffer, input + rcursor, token.length);
			
			{
				char tok = token.type;
				dm_write(&bytecode, &tok, 1); 
			}
			
			switch(token.type)
			{
				case TOK_INTEGER:
				{
					int ival = str_to_int(buffer, 10);
					dm_write(&bytecode, &ival, sizeof(ival));
					break;
				}
				case TOK_BOOL:
				{
					bool bval = str_eq(buffer, "True") || str_eq(buffer, "On");
					dm_write(&bytecode, &bval, sizeof(bval));
					break;
				}
				// Both strings are stored with each length+ptr and zero termination.
				case TOK_STRING:
				{
					int len = token.length - 2;
					dm_write(&bytecode, &len, sizeof(len));
					dm_write(&bytecode, buffer + 1, len);
					dm_write(&bytecode, &zero, sizeof(zero));
					break;
				}
				// Both strings are stored with each length+ptr and zero termination.
				case TOK_FUN:
				case TOK_VAR:
				case TOK_ORDER:
				{
					int len = token.length;
					dm_write(&bytecode, &len, sizeof(len));
					dm_write(&bytecode, buffer, len);
					dm_write(&bytecode, &zero, sizeof(zero));
					break;
				}
			}
			if(token.type == TOKEN_EMPTY)
				break;
		}
		else if(token.type == TOKEN_INVALID)
		{
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
			dm_write(&bytecode, &end, 1);
			
			rcursor++;
			
			if(rcursor >= insize)
				break;
		}
	}
	
	// Finally, put the stuff in our target file.
	file_resize(outfile, bytecode.cursor);
	mem_copy(
		file_data(outfile),
		bytecode.ptr,
		bytecode.cursor);
	
	dm_kill(&bytecode);
}