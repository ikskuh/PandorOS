#include "interpreter.h"
#include "io.h"
#include "pmm.h"
#include "longjmp.h"
#include "standard.h"
#include "debug.h"
#include "basic/lexer.h"


#define cmd_eq(a, b) str_eqi((a), (b))

// declared in grammar.lg
extern bool errorhandler_valid;
extern jmp_buf errorhandler;
	
/**
 * Throws an error.
 */
void basic_error(error_t reason)
{
	if(reason == ERR_SUCCESS) return;
	longjmp(errorhandler, reason);
	printf("?longjmp?");
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

typedef struct label
{
	char label[64];
	int location;
	struct label * next;
} label_t;

dynmem_t basic_compile(char const * input, int insize)
{
	char zero = 0;

	dynmem_t bytecode = dynmem_new();
	
	label_t *labels = NULL;
	label_t *patches = NULL;
	
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
		if(
			token.type != TOKEN_WHITESPACE && 
			token.type != TOKEN_INVALID)
		{
			char buffer[token.length + 1];
			mem_set(buffer, 0, token.length + 1);
			mem_copy(buffer, input + rcursor, token.length);
			
			if(token.type == TOKEN_LABEL)
			{
				int start ;
				for(
					start = token.length - 1; 
					start >= 0 && buffer[start] != ' '; 
					start--);
				start++;
				
				label_t * label = zalloc(sizeof(label_t));
				label->location = bytecode.cursor;
				str_copy(label->label, (&buffer[start]));
				label->next = labels;
				labels = label;
			}
			else
			{
				char tok = token.type;
				dynmem_write(&bytecode, &tok, 1); 
			
				switch(token.type)
				{
					case TOKEN_IF:
					{
						// An if just says that the following expression
						// will be executed dependently or not.
						break;
					}
					case TOKEN_GOTO:
					{
						int start ;
						for(
							start = token.length - 1; 
							start >= 0 && buffer[start] != ' '; 
							start--);
						start++;
						
						label_t * patch = zalloc(sizeof(label_t));
						patch->location = bytecode.cursor;
						str_copy(patch->label, &buffer[start]);
						patch->next = patches;
						patches = patch;
						
						int dest = 0xFFFFFFFF;
						dynmem_write(&bytecode, &dest, sizeof(dest));
					
						break;
					}
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
	
	// Patch all goto references
	for(label_t *patch = patches; patch != NULL; patch = patch->next)
	{
		label_t *label = NULL;
		for(label_t *l = labels; l != NULL; l = l->next)
		{
			if(str_eq(l->label, patch->label)) {
				label = l;
				break;
			}
		}
		if(label == NULL)
			basic_error(ERR_INVALID_LABEL);
		
		int *dest = (void*)&bytecode.ptr[patch->location];
		*dest = label->location;
	}
	
	// Free all labels and jumps:
	for(label_t *l = labels; l != NULL;)
	{
		label_t *k = l;
		l = l->next;
		free(k);
	}
	for(label_t *l = patches; l != NULL;)
	{
		label_t *k = l;
		l = l->next;
		free(k);
	}
	
	// hexdump("Tokenized Code", bytecode.ptr, bytecode.cursor);
	
	return bytecode;
}


value_t basic_execute(char const *input)
{
	int len = str_len(input);
	
	dynmem_t bytecode = basic_compile(input, len);
	
	// TODO: Fix memory leak on error
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
		if(cmd_eq(name, functions[i].name))
			return functions[i].func;
	}
	return NULL;
}